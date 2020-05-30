
#include "XZRect.h"
#include "Material.h"
#include "AABB.h"
#include "Helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
XZRect::XZRect(const glm::vec3& _center, float _width, float _depth, Material* ptr_mat)
{
	center = _center;
	width = _width;
	depth = _depth;

	// calculate bounds
	x0 = center.x - (width * 0.5f);
	x1 = center.x + (width * 0.5f);
	z0 = center.z - (depth * 0.5f);
	z1 = center.z + (depth * 0.5f);
	y = center.y;

	mat_ptr = ptr_mat;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool XZRect::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	float t = (y - r.origin.y) / (r.direction.y);
	if (t < tmin || t > tmax)
		return false;

	float x = r.origin.x + t * r.direction.x;
	float z = r.origin.z + t * r.direction.z;

	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;

	rec.P = r.GetPointAt(t);
	rec.uv.x = (x - x0) / (x1 - x0);
	rec.uv.y = (z - z0) / (z1 - z0);
	rec.t = t;

	glm::vec3 outward_normal = glm::vec3(0, 1, 0);
	if (glm::dot(r.direction, outward_normal) < 0.0f)
		rec.N = outward_normal;
	else
		rec.N = -outward_normal;

	rec.mat_ptr = mat_ptr;
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void XZRect::BoundingBox(AABB& box) const
{
	box = AABB(glm::vec3(x0, y - 0.0001f, z0), glm::vec3(x1, y + 0.0001f, z1));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
float XZRect::PDF(const glm::vec3& origin, const glm::vec3& direction) const
{
	HitRecord rec;
	if (!this->hit(Ray(origin, direction), 0.0001f, FLT_MAX, rec))
		return 0.0f;

	float area = (x1 - x0) * (z1 - z0);
	float distSqrd = rec.t * rec.t * glm::length2(direction);
	float cosine = std::fabsf(glm::dot(direction, rec.N) / glm::length(direction));

	return distSqrd / (cosine * area);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 XZRect::Sample(const glm::vec3& origin) const
{
	glm::vec3 randomPoint = glm::vec3(Helper::GetRandomRange(x0, x1), y, Helper::GetRandomRange(z0, z1));
	return randomPoint - origin;
}




