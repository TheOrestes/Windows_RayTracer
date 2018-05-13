#pragma once

#include "Vector3.h"

class Ray
{
public:
	Ray() {}
	Ray(const Vector3& A, const Vector3& B) 
	{ 
		origin = A;
		direction = B; 
	}

	Vector3 GetRayOrigin() const { return origin; }
	Vector3 GetRayDirection() const { return direction; }
	Vector3 GetPointAt(float t) const { return origin + t * direction; }

private:
	Vector3 origin;
	Vector3 direction;
};