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

	inline Vector3 GetRayOrigin() const { return origin; }
	inline Vector3 GetRayDirection() const { return direction; }
	inline Vector3 GetPointAt(float t) const { return origin + t * direction; }

private:
	Vector3 origin;
	Vector3 direction;
};