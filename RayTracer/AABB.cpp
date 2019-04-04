
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

	bool xHit = true; 
	bool yHit = true;
	bool zHit = true;

	glm::vec3 rayOrigin = r.GetRayOrigin();
	glm::vec3 rayDirection = r.GetRayDirection();
	glm::vec3 rayInvDirection = r.GetInvRayDirection();
	
	// Direction X
	float t0x = (minBound[0] - rayOrigin[0]) * rayInvDirection[0];
	float t1x = (maxBound[0] - rayOrigin[0]) * rayInvDirection[0];
	if (rayInvDirection[0] < 0.0f)
		std::swap(t0x, t1x);
	
	tmin = (t0x > tmin) ? t0x : tmin;
	tmax = (t1x < tmax) ? t1x : tmax;
	if (tmax <= tmin)
		xHit = false;
	
	// Y Direction
	float t0y = (minBound[1] - rayOrigin[1]) * rayInvDirection[1];
	float t1y = (maxBound[1] - rayOrigin[1]) * rayInvDirection[1];
	if (rayInvDirection[1] < 0.0f)
		std::swap(t0y, t1y);
	
	tmin = (t0y > tmin) ? t0y : tmin;
	tmax = (t1y < tmax) ? t1y : tmax;
	if (tmax <= tmin)
		yHit = false;
	
	// Z Direction
	float t0z = (minBound[2] - rayOrigin[2]) * rayInvDirection[2];
	float t1z = (maxBound[2] - rayOrigin[2]) * rayInvDirection[2];
	if (rayInvDirection[1] < 0.0f)
		std::swap(t0z, t1z);
	
	tmin = (t0z > tmin) ? t0z : tmin;
	tmax = (t1z < tmax) ? t1z : tmax;
	if (tmax <= tmin)
		zHit = false;

	if (xHit && yHit && zHit)
	{
		++rec.rayBoxSuccess;
		return true;
	}
	else
		return false;
}
