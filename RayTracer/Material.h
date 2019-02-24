#pragma once

#include "Ray.h"
#include "Hitable.h"

class Material
{
public:
	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& attenuation, Ray& scattered) const = 0;
};
