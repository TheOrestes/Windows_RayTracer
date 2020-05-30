
#include "XYRect.h"
#include "Material.h"
#include "AABB.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
XYRect::XYRect(const glm::vec3& _center, float _width, float _height, Material* ptr_mat)
{
	center = _center;
	width = _width;
	height = _height;

	// calculate bounds
	x0 = center.x - (width * 0.5f);
	x1 = center.x + (width * 0.5f);
	y0 = center.y - (height * 0.5f);
	y1 = center.y + (height * 0.5f);
	z = center.z;

	mat_ptr = ptr_mat;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool XYRect::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	float t = (z - r.origin.z) / (r.direction.z);
	if (t < tmin || t > tmax)
		return false;

	float x = r.origin.x + t * r.direction.x;
	float y = r.origin.y + t * r.direction.y;

	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;

	rec.P = r.GetPointAt(t);
	rec.uv.x = (x - x0) / (x1 - x0);
	rec.uv.y = (y - y0) / (y1 - y0);
	rec.t = t;

	glm::vec3 outward_normal = glm::vec3(0, 0, 1);
	if (glm::dot(r.direction, outward_normal) < 0.0f)
		rec.N = outward_normal;
	else
		rec.N = -outward_normal;

	rec.mat_ptr = mat_ptr;
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void XYRect::BoundingBox(AABB& box) const
{
	box = AABB(glm::vec3(x0, y0, z - 0.0001f), glm::vec3(x1, y1, z + 0.0001f));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
float XYRect::PDF(const glm::vec3& origin, const glm::vec3& direction) const
{
	return 1.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 XYRect::Sample(const glm::vec3& origin) const
{
	return glm::vec3(1, 1, 1);
}




