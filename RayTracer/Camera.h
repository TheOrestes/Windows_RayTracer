#pragma once

#include "Ray.h"
#include "Helper.h"

enum eProjectionType
{
	OTHOGRAPHIC,
	PERSPECTIVE,
	FISHEYE,
	SPHERICAL,
};

class Camera
{
public:
	Camera();
	~Camera();

	void InitCamera(const glm::vec3& _position, const glm::vec3& _lookAt, float _screenWidth, float _screenHeight);
	Ray get_ray(float s, float t);

private:
	// Camera vectors
	glm::vec3		position, lookAt, Up;

	// basis vectors
	glm::vec3		u, v, w;

	// screen params
	float			halfWidth;
	float			halfHeight;
	float			screenWidht;
	float			screenHeight;

	// For Perspective
	float			viewPlaneDistance;

	// For Fisheye
	float			psi_max;

	// For Spherical
	float			lambda_max;

	eProjectionType projectionType;
};
