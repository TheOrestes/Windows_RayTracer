#pragma once

#include "Ray.h"

class Material;
class AABB;

struct HitRecord
{
	HitRecord()
	{
		t = 0.0f;
		P = glm::vec3(0.0f, 0.0f, 0.0f);
		N = glm::vec3(0.0f, 0.0f, 0.0f);
		uv = glm::vec2(0.0f, 0.0f);
		mat_ptr = nullptr;

		rayTriangleQuery = 0;
		rayTriangleSuccess = 0;
		rayBoxQuery = 0;
		rayBoxSuccess = 0;
	}

	float t;
	glm::vec3 P;
	glm::vec3 N;
	glm::vec2 uv;
	Material* mat_ptr;

	// Debug...
	uint64_t rayTriangleQuery;
	uint64_t rayTriangleSuccess;
	uint64_t rayBoxQuery;
	uint64_t rayBoxSuccess;
};

class Hitable
{
public:
	virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
	virtual void BoundingBox(AABB& box) const = 0;
};