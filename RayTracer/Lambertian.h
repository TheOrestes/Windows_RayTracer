#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"

class Lambertian : public Material
{
public:
	Lambertian(const glm::vec3& _albedo) : Albedo(_albedo) {}

	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scatterd) const;

private:
	glm::vec3 Albedo;
};