#pragma once

#include "Ray.h"

class Material;

struct HitRecord
{
	float t;
	glm::vec3 P;
	glm::vec3 N;
	Material* mat_ptr;
};

class Hitable
{
public:
	virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
};