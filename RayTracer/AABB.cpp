
#include "Hitable.h"
#include "AABB.h"
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////////////////////
AABB::AABB()
{
	minBound = glm::vec3(0);
	maxBound = glm::vec3(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void AABB::UpdateBB(const glm::vec3& _pos)
{
	if (_pos.x < minBound.x) { minBound.x = _pos.x; }
	if (_pos.y < minBound.y) { minBound.y = _pos.y; }
	if (_pos.z < minBound.z) { minBound.z = _pos.z; }

	if (_pos.x > maxBound.x) { maxBound.x = _pos.x; }
	if (_pos.y > maxBound.y) { maxBound.y = _pos.y; }
	if (_pos.z > maxBound.z) { maxBound.z = _pos.z; }
}

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
	float t0x = (minBound.x - rayOrigin.x) * rayInvDirection.x;
	float t1x = (maxBound.x - rayOrigin.x) * rayInvDirection.x;
	if (rayInvDirection.x < 0.0f)
		std::swap(t0x, t1x);
	
	tmin = (t0x > tmin) ? t0x : tmin;
	tmax = (t1x < tmax) ? t1x : tmax;
	if (tmax <= tmin)
		xHit = false;
	
	// Y Direction
	float t0y = (minBound.y - rayOrigin.y) * rayInvDirection.y;
	float t1y = (maxBound.y - rayOrigin.y) * rayInvDirection.y;
	if (rayInvDirection.y < 0.0f)
		std::swap(t0y, t1y);
	
	tmin = (t0y > tmin) ? t0y : tmin;
	tmax = (t1y < tmax) ? t1y : tmax;
	if (tmax <= tmin)
		yHit = false;
	
	// Z Direction
	float t0z = (minBound.z - rayOrigin.z) * rayInvDirection.z;
	float t1z = (maxBound.z - rayOrigin.z) * rayInvDirection.z;
	if (rayInvDirection.z < 0.0f)
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
