
#include "Sphere.h"

/////////////////////////////////////////////////////////////////////////////////////////
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
		t = (-b - sqrt(discriminant)) / (2.0 * a);
		if (t < tmax && t > tmin)
		{
			rec.t = t;
			rec.P = r.GetPointAt(t);
			rec.N = (rec.P - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}

		t = (-b + sqrt(discriminant)) / (2.0 * a);
		if (t < tmax && t > tmin)
		{
			rec.t = t;
			rec.P = r.GetPointAt(t);
			rec.N = (rec.P - center) / radius;
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}

	return false;
}