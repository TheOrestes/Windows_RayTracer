#pragma once

#include "Hitable.h"

class Material;

class Sphere : public Hitable
{
public:
	Sphere() {}
	Sphere(const glm::vec3& _center, float _r, Material* ptr_mat) :
		center(_center),
		radius(_r),
		mat_ptr(ptr_mat) {};

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
	virtual void BoundingBox(AABB& box) const;

	glm::vec2 GetSphereUV(const glm::vec3& p) const;

private:
	glm::vec3 center;
	float radius;
	Material* mat_ptr;
};

