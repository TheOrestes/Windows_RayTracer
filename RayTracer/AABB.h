#pragma once

#include "glm/glm.hpp"
#include "Ray.h"

struct HitRecord;

enum eLongestAxis
{
	X_AXIS = 0,
	Y_AXIS,
	Z_AXIS
};

class AABB
{
public:
	AABB();
	~AABB() {}
	AABB(const glm::vec3& _min, const glm::vec3& _max) :
		minBound(_min),
		maxBound(_max) {}

	int			GetLongestAxis();
	glm::vec3	GetCentroidPoint();

	void		ExpandBoundingBox(const AABB& _box);
	AABB		GetSurroundingBBox(const AABB& _box0, const AABB& _box1);

	bool		hit(const Ray& r, float tmin, float tmax, HitRecord& rec);

	glm::vec3	minBound;		// top back left
	glm::vec3	maxBound;		// bottom right front
};