
#include "Sphere.h"
#include "Helper.h"
#include "AABB.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Sphere::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	glm::vec3 rayDirection = r.direction;
	glm::vec3 rayOrigin = r.origin;

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
float Sphere::PDF(const glm::vec3& origin, const glm::vec3& direction) const
{
	HitRecord rec;

	if(!this->hit(Ray(origin, direction), 0.001f, FLT_MAX, rec))
		return 0.0f;

	float cosThetaMax = sqrtf(1.0f - ((radius * radius) / (glm::length2(center - origin))));
	float solidAngle = 2.0f * PI * (1.0f - cosThetaMax);

	return 1.0f / solidAngle;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Sphere::Sample(const glm::vec3& origin) const
{
	glm::vec3 dir = center - origin;
	float distSqrd = glm::length2(dir);
	return dir / distSqrd;// Helper::SphereLightSampling(dir, radius, distSqrd);
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
