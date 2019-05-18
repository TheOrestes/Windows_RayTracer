#pragma once

#include "Ray.h"
#include "Helper.h"

class Camera
{
public:
	Camera();
	~Camera();

	void InitCamera(const glm::vec3& _position, const glm::vec3& _lookAt, float _screenWidth, float _screenHeight);
	Ray get_ray(float s, float t);

private:
	glm::vec3 position, lookAt, Up;
	glm::vec3 origin;
	glm::vec3 lower_left_corner;
	glm::vec3 horizontal;
	glm::vec3 vertical;
	glm::vec3 u, v, w;
	float lens_radius, aperture, vfov, focus_dist;
};
