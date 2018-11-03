#pragma once

#include "glm/glm.hpp"

class Ray
{
public:
	Ray() {}
	Ray(const glm::vec3& A, const glm::vec3& B) 
	{ 
		origin = A;
		direction = B; 
	}

	inline glm::vec3 GetRayOrigin() const { return origin; }
	inline glm::vec3 GetRayDirection() const { return direction; }
	inline glm::vec3 GetPointAt(float t) const { return origin + t * direction; }

private:
	glm::vec3 origin;
	glm::vec3 direction;
};