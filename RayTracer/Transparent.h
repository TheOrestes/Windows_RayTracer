#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Helper.h"
#include "Material.h"
#include "Texture.h"

class Transparent : public Material
{
public:
	Transparent(Texture* _albedo, float ri) : Albedo(_albedo), refr_index(ri) {}
	
	virtual bool		Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scattered, float& pdf) const;

private:

	glm::vec3			BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const;

	Texture*	Albedo;
	float		refr_index;
};