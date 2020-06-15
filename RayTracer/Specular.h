#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Texture.h"

class Specular : public Material
{
public:
	Specular(Texture* _albedo, float _roughness, float _ks)
		: Albedo(_albedo),
		Roughness(_roughness),
		Ks(_ks),
		Kd(1 - _ks),
		Material(false, MaterialType::SPECULAR) {
		NDF = 0.0f; 
		PDF = 0.0f;
	}
	
	virtual bool		Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd, float& pdf) const;

private:

	glm::vec3			BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered, const glm::vec3& Half) const;

	// Microfacet models...
	void				NormalDistributionTerm_GGX(const Ray& r_in, const HitRecord& rec, const glm::vec3& lightDir, const glm::vec3& Half, float& D, float& pdf) const;
	void				NormalDistributionTerm_Beckmann(const Ray& r_in, const HitRecord& rec, const Ray& scattered, float& D, float& pdf) const;
	void				NormalDistributionTerm_Phong(const Ray& r_in, const HitRecord& rec, const Ray& scattered, float& D, float& pdf) const;
	float				GeometricAttenuationTerm(const Ray& r_in, const HitRecord& rec, const glm::vec3& lightDir) const;
	float				FresnelTerm(const glm::vec3& Half, const glm::vec3& viewDir) const;

	Texture*			Albedo;
	float				Roughness;

	float				Ks;
	float				Kd;

	float				NDF;
	float				PDF;
};
