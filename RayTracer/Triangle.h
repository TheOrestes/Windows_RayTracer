#pragma once

#pragma once

#include "Hitable.h"

class Material;

class Triangle : public Hitable
{
public:
	Triangle() {}
	Triangle(Vector3 _v0, Vector3 _v1, Vector3 _v2, Material* ptr_mat) :
		V0(_v0),
		V1(_v1),
		V2(_v2),
		mat_ptr(ptr_mat) {};

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;

private:
	Vector3 V0;
	Vector3 V1;
	Vector3 V2;
	Material* mat_ptr;
};
