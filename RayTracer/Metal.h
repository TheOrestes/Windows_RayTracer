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

	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scatterd) const;

private:
	Texture* Albedo;
	float fuzz;
};
