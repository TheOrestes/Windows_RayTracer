#pragma once

#include "Hitable.h"

class Material;

class XYRect : public Hitable
{
public:
	XYRect() {};
	XYRect(const glm::vec3& _center, float _width, float _height, Material* ptr_mat);
			
	~XYRect() {};

	virtual bool		hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
	virtual void		BoundingBox(AABB& box) const;

	virtual float		PDF(const glm::vec3& origin, const glm::vec3& direction) const;
	virtual glm::vec3	Sample(const glm::vec3& origin) const;

public:
	glm::vec3 center;
	float width;
	float height;

private:
	Material* mat_ptr;
	float x0, x1, y0, y1, z;
};
