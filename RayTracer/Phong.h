#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Texture.h"

class Phong : public Material
{
public:
	Phong(Texture* _albedo, float _power, float _ks) 
			:	Albedo(_albedo), 
				SpecularPower(_power),
				Ks(_ks)
	{
		Kd = 1.0f - Ks;
	}

	virtual bool		Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd) const;
	virtual float		PDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const;

private:

	glm::vec3			BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const;

	Texture*	Albedo;
	float		SpecularPower;

	float		Ks;
	float		Kd;
};
