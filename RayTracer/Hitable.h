#pragma once

#include "Ray.h"

class Material;

struct HitRecord
{
	HitRecord()
	{
		t = 0.0f;
		P = glm::vec3(0);
		N = glm::vec3(0);
		uv = glm::vec2(0);
		mat_ptr = nullptr;

		rayTriangleTestCount = 0;
		rayTriangleIntersectionCount = 0;
		rayBoxTestCount = 0;
	}

	float t;
	glm::vec3 P;
	glm::vec3 N;
	glm::vec2 uv;
	Material* mat_ptr;

	// Debug...
	int rayTriangleTestCount;
	int rayTriangleIntersectionCount;
	int rayBoxTestCount;
};

class Hitable
{
public:
	virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
};