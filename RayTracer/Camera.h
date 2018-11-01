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
	Vector3 lookFrom, lookAt, Up;
	Vector3 origin;
	Vector3 lower_left_corner;
	Vector3 horizontal;
	Vector3 vertical;
	Vector3 u, v, w;
	float lens_radius, aperture, vfov, focus_dist;
};
