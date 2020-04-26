#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Texture.h"

class Metal : public Material
{
public:
	Metal (Texture* _albedo, float f) : Albedo(_albedo) 
	{
		if (f < 1)
			fuzz = f;
		else
			fuzz = 1;
	}

	virtual bool		Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd) const;
	virtual float		PDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const;

private:

	glm::vec3			BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const;

	Texture*	Albedo;
	float		fuzz;
};
