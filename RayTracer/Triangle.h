#pragma once

#include "glm\glm.hpp"
#include "Hitable.h"

class Material;

class Triangle : public Hitable
{
public:
	Triangle() {}
	Triangle(const glm::vec3& _v0, const glm::vec3& _v1, const glm::vec3& _v2, Material* ptr_mat) :
		V0(_v0),
		V1(_v1),
		V2(_v2),
		mat_ptr(ptr_mat) {};

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;

private:
	glm::vec3 V0;
	glm::vec3 V1;
	glm::vec3 V2;
	Material* mat_ptr;
};
