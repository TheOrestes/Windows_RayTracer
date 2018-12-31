#pragma once

#include "glm\glm.hpp"
#include "Hitable.h"
#include "..\Main\VertexStructures.h"

class Material;

class Triangle : public Hitable
{
public:
	Triangle() {}
	Triangle(const VertexPNT& _v0, const VertexPNT& _v1, const VertexPNT& _v2, Material* ptr_mat) :
		v0(_v0), v1(_v1), v2(_v2),
		mat_ptr(ptr_mat) {};

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;

private:
	VertexPNT v0;
	VertexPNT v1;
	VertexPNT v2;
	Material* mat_ptr;
};
