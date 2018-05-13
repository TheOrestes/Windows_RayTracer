#pragma once

#include "Ray.h"
#include "Helper.h"

class Camera
{
public:
	Camera(Vector3 lookFrom, Vector3 lookAt, Vector3 Up, float vfov, float aspect, float aperture, float focus_dist)	// vofv is vertical fov
	{
		lens_radius = aperture / 2.0f;

		float theta = vfov * PI / 180.0f;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;

		origin = lookFrom;
		w = unit_vector(lookFrom - lookAt);
		u = unit_vector(cross(Up, w));
		v = cross(w, u);

		lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
		horizontal = 2 * half_width * focus_dist * u;
		vertical = 2 * half_height * focus_dist * v;
	}

	Ray get_ray(float s, float t)
	{
		Vector3 rd = lens_radius * Helper::GetRandomInUnitDisk();
		Vector3 offset = rd.x * u + rd.y * v;
		return Ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
	}

private:
	Vector3 origin;
	Vector3 lower_left_corner;
	Vector3 horizontal;
	Vector3 vertical;
	Vector3 u, v, w;
	float lens_radius;
};
