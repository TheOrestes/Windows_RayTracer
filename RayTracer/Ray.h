#pragma once

#include "glm/glm.hpp"

class Ray
{
public:
	Ray() 
	{
		origin = glm::vec3(0.0f, 0.0f, 0.0f);
		direction = glm::vec3(1.0f, 1.0f, 1.0f);
		invDirection = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	Ray(const glm::vec3& A, const glm::vec3& B) 
	{ 
		origin = A;
		direction = B; 

		invDirection = glm::vec3(1.0f / direction.x, 1.0f/direction.y, 1.0f/direction.z);
	}

	inline glm::vec3 GetRayOrigin() const { return origin; }
	inline glm::vec3 GetRayDirection() const { return direction; }
	inline glm::vec3 GetInvRayDirection() const { return invDirection; }
	inline glm::vec3 GetPointAt(float t) const { return origin + t * direction; }

private:
	glm::vec3 origin;
	glm::vec3 direction;
	glm::vec3 invDirection;
};