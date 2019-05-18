
#include "Sphere.h"
#include "Helper.h"
#include "AABB.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Sphere::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	glm::vec3 rayDirection = r.GetRayDirection();
	glm::vec3 rayOrigin = r.GetRayOrigin();

	glm::vec3 oc = rayOrigin - center;
	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(oc, rayDirection);
	float c = glm::dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a* c;

	float t;

	if (discriminant > 0)
	{
		t = (-b - sqrt(discriminant)) / (2.0f * a);
		if (t < tmax && t > tmin)
		{
			rec.t = t;
			rec.P = r.GetPointAt(t);
			rec.N = (rec.P - center) / radius;
			rec.uv = GetSphereUV((rec.P - center) / radius);

			rec.mat_ptr = mat_ptr;
			return true;
		}

		t = (-b + sqrt(discriminant)) / (2.0f * a);
		if (t < tmax && t > tmin)
		{
			rec.t = t;
			rec.P = r.GetPointAt(t);
			rec.N = (rec.P - center) / radius;
			rec.uv = GetSphereUV((rec.P - center) / radius);

			rec.mat_ptr = mat_ptr;
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Sphere::BoundingBox(AABB & box) const
{
	box = AABB(center - glm::vec3(radius, radius, radius), center + glm::vec3(radius, radius, radius));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec2 Sphere::GetSphereUV(const glm::vec3& p) const
{
	float phi = std::atan2(p[2], p[0]);
	float theta = std::asin(p[1]);

	float x = 1 - (phi + PI) / (2 * PI);
	float y = (theta + PI / 2) / PI;

	return glm::vec2(x, y);
}
