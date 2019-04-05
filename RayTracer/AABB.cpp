
#include "Hitable.h"
#include "AABB.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////////////////////
AABB::AABB()
{
	minBound = glm::vec3(0.0f, 0.0f, 0.0f);
	maxBound = glm::vec3(0.0f, 0.0f, 0.0f);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
int AABB::GetLongestAxis()
{
	glm::vec3 axis = maxBound - minBound;

	if (axis[0] > axis[1] && axis[0] > axis[2]) return eLongestAxis::X_AXIS;
	if (axis[1] > axis[0] && axis[1] > axis[2]) return eLongestAxis::Y_AXIS;
	if (axis[2] > axis[0] && axis[2] > axis[1]) return eLongestAxis::Z_AXIS;

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 AABB::GetCentroidPoint()
{
	return glm::vec3(0.5f * minBound + 0.5f * maxBound);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void AABB::ExpandBoundingBox(const AABB& _box)
{
	minBound = glm::vec3(fminf(_box.minBound[0], minBound[0]), fminf(_box.minBound[1], minBound[1]), fminf(_box.minBound[2], minBound[2]));
	maxBound = glm::vec3(fmaxf(_box.maxBound[0], maxBound[0]), fmaxf(_box.maxBound[1], maxBound[1]), fmaxf(_box.maxBound[2], maxBound[2]));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
AABB AABB::GetSurroundingBBox(const AABB & _box0, const AABB & _box1)
{
	glm::vec3 minBound = glm::vec3(fminf(_box0.minBound[0], _box1.minBound[0]), fminf(_box0.minBound[1], _box1.minBound[1]), fminf(_box0.minBound[2], _box1.minBound[2]));
	glm::vec3 maxBound = glm::vec3(fmaxf(_box0.maxBound[0], _box1.maxBound[0]), fmaxf(_box0.maxBound[1], _box1.maxBound[1]), fmaxf(_box0.maxBound[2], _box1.maxBound[2]));

	return AABB(minBound, maxBound);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABB::hit(const Ray & r, float tmin, float tmax, HitRecord& rec)
{
	++rec.rayBoxQuery;

	for (int a = 0; a < 3; a++)
	{
		float t0 = fminf((minBound[a] - r.GetRayOrigin()[a]) / r.GetRayDirection()[a],
						 (maxBound[a] - r.GetRayOrigin()[a]) / r.GetRayDirection()[a]);

		float t1 = fmaxf((minBound[a] - r.GetRayOrigin()[a]) / r.GetRayDirection()[a],
						 (maxBound[a] - r.GetRayOrigin()[a]) / r.GetRayDirection()[a]);

		tmin = fmaxf(t0, tmin);
		tmax = fminf(t1, tmax);

		if (tmax <= tmin)
			return false;
	}

	++rec.rayBoxSuccess;
	return true;
}
