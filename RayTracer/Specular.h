#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Texture.h"

class Specular : public Material
{
public:
	Specular(Texture* _albedo, float _roughness, float _power, float _ks)
				: Albedo(_albedo),
				Roughness(_roughness),
				SpecularPower(_power),
				Ks(_ks),
				Kd(1 - _ks),
				m_bIsLightSource(false) {}
	
	virtual bool		Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd) const;
	virtual float		PDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const;

private:

	glm::vec3			BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const;

	// Microfacet models...
	float				NormalDistributionTerm_GGX() const;
	float				NormalDistributionTerm_Beckmann() const;
	float				NormalDistributionTerm_Phong() const;
	float				GeometricAttenuationTerm() const;
	float				FresnelTerm() const;

	Texture*			Albedo;
	float				Roughness;
	float				SpecularPower;

	float				Ks;
	float				Kd;

public:
	bool				m_bIsLightSource;
};
