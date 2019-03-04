#pragma once

#include "glm\glm.hpp"
#include "Ray.h"

struct HitRecord;

class AABB
{
public:
	AABB();
	~AABB() {}
	AABB(const glm::vec3& _min, const glm::vec3& _max) :
		minBound(_min),
		maxBound(_max) {}

	void UpdateBB(const glm::vec3& _pos);
	bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec);

private:
	glm::vec3 minBound;
	glm::vec3 maxBound;
};