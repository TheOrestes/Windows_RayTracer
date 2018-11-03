#pragma once

#include "Ray.h"
#include "Helper.h"

class Camera
{
public:
	static Camera& getInstance()
	{
		static Camera instance;
		return instance;
	}

	~Camera();

	void InitCamera(float screenWidth, float screenHeight);
	Ray get_ray(float s, float t);

private:
	Camera();
	glm::vec3 lookFrom, lookAt, Up;
	glm::vec3 origin;
	glm::vec3 lower_left_corner;
	glm::vec3 horizontal;
	glm::vec3 vertical;
	glm::vec3 u, v, w;
	float lens_radius, aperture, vfov, focus_dist;
};
