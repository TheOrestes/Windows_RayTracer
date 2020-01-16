
#include <Windows.h>
#include "Camera.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
Camera::Camera()
{
	position = glm::vec3(0.0f, 2.0f, 0.0f);
	lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	Up = glm::vec3(0.0f, 1.0f, 0.0f);

	viewPlaneDistance = 0.0f;

	screenWidht = 0.0f;
	screenHeight = 0.0f;

	psi_max = 180.0f;
	lambda_max = 180.0f;

	projectionType = eProjectionType::PERSPECTIVE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Camera::~Camera()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Camera::InitCamera(const glm::vec3& _position, const glm::vec3& _lookAt, float _screenWidth, float _screenHeight)
{
	// Camera params
	position = _position;
	lookAt = _lookAt;

	// Common screen params
	screenWidht = _screenWidth;
	screenHeight = _screenHeight;
	halfHeight = _screenHeight * 0.5f;
	halfWidth = _screenWidth * 0.5f;

	// Basic vectors
	// For OpenGL, order of Cross product changes!
	w = glm::normalize(position - lookAt);
	u = glm::normalize(glm::cross(w, Up));
	v = glm::normalize(glm::cross(u, w));

	switch (projectionType)
	{
		case eProjectionType::PERSPECTIVE:
		{
			// FOV & Distance "d" from Eye position to View Plane are inter-dependent! 
			// We can use any one as per user convenience. 
			// tan(fov/2) = ( 0.5 * screenHeight ) / d;
			// or
			// d = screenHeight / 2 * tan(fov/2)
			// We will be using "d" for the sake of it. 
			viewPlaneDistance = 400.0f;
			break;
		}

		case eProjectionType::FISHEYE:
		{
			break;
		}

		case eProjectionType::SPHERICAL:
		{
			break;
		}

		case eProjectionType::OTHOGRAPHIC:
		{
			u = glm::vec3(1, 0, 0);
			v = glm::vec3(0, 1, 0);

			// For OpenGL, order of Cross product changes!		

			break;
		}

		default:
		{
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Ray Camera::get_ray(float s, float t)
{
	Ray ray;
	ray.origin = glm::vec3(0);
	ray.direction = glm::vec3(0);

	switch (projectionType)
	{
		case eProjectionType::PERSPECTIVE:
		{
			ray.origin = position;
			
			glm::vec3 pos = glm::vec3(s - halfWidth, t - halfHeight, 0.0f);
			ray.direction = pos.x * u + pos.y * v - viewPlaneDistance * w;

			break;
		}

		case eProjectionType::FISHEYE:
		{
			ray.origin = position;

			// Compute Normalized Device coordinates
			glm::vec2 ndc(0, 0);
			ndc.x = (2.0f / screenWidht) * (s - halfWidth);
			ndc.y = (2.0f / screenHeight) * (t - halfHeight);

			float r_squared = ndc.x * ndc.x + ndc.y * ndc.y;

			if (r_squared <= 1.0f)
			{
				float r = sqrt(r_squared);
				float psi = glm::radians(r * psi_max);
				float sin_psi = sin(psi);
				float cos_psi = cos(psi);
				float sin_alpha = ndc.y / r;
				float cos_alpha = ndc.x / r;

				ray.direction = sin_psi * cos_alpha * u + sin_psi * sin_alpha * v - cos_psi * w;
			}

			break;
		}

		case eProjectionType::SPHERICAL:
		{
			ray.origin = position;

			// Compute Normalized Device coordinates
			glm::vec2 ndc(0, 0);
			ndc.x = (2.0f / screenWidht) * (s - halfWidth);
			ndc.y = (2.0f / screenHeight) * (t - halfHeight);

			// compute angles lambda & phi in radians
			float lambda = glm::radians(ndc.x * lambda_max);
			float psi = glm::radians(ndc.y * psi_max);

			// Compute the spherical azimuth & polar angles
			float phi = PI - lambda;
			float theta = 0.5f * PI - psi;

			float sin_phi = sin(phi);
			float cos_phi = cos(phi);
			float sin_theta = sin(theta);
			float cos_theta = cos(theta);

			ray.direction = sin_theta * sin_phi * u + cos_theta * v + sin_theta * cos_phi * w;

			break;
		}

		case eProjectionType::OTHOGRAPHIC:
		{
			float pixelScale = 1.0f; // controls zoom level

			ray.origin = glm::vec3(pixelScale * (s - halfWidth), pixelScale * (t - halfHeight), 0.0f);
			ray.direction = glm::vec3(0, 0, -1);

			break;
		}

		default:
		{
			break;
		}
	}

	return ray;
}

