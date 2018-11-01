
#include <Windows.h>
#include "Camera.h"

Camera::Camera()
{
	aperture = 0.0f;
	focus_dist = 1.0f;
	vfov = 45.0f;
}

Camera::~Camera()
{

}

void Camera::InitCamera(float screenWidth, float screenHeight)
{
	lookFrom = Vector3(4.0f, 4.0f, 7.0f);
	lookAt = Vector3(0.0f, 2.0f, 0.0f);
	Up = Vector3(0.0f, 1.0f, 0.0f);

	lens_radius = aperture / 2.0f;
	 
	float theta = vfov * PI / 180.0f;
	float half_height = tan(theta / 2);
	float half_width = (screenWidth / screenHeight) * half_height;

	origin = lookFrom;
	w = UnitVector(lookFrom - lookAt);
	u = UnitVector(Cross(Up, w));
	v = Cross(w, u);

	lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
	horizontal = 2 * half_width * focus_dist * u;
	vertical = 2 * half_height * focus_dist * v;
}

Ray Camera::get_ray(float s, float t)
{
	Vector3 rd = lens_radius * Helper::GetRandomInUnitDisk();
	Vector3 offset = rd.x * u + rd.y * v;
	return Ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
}

