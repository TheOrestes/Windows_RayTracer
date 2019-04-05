#pragma once

#include "glm/glm.hpp"
#include "Hitable.h"
#include "..\Main\VertexStructures.h"

class Material;

class Triangle : public Hitable
{
public:
	Triangle() {}
	Triangle(const VertexPNT& _v0, const VertexPNT& _v1, const VertexPNT& _v2, Material* ptr_mat);

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
	virtual void BoundingBox(AABB& box) const;

	inline glm::vec3 Centroid() const { return centroid; }

private:
	VertexPNT v0;
	VertexPNT v1;
	VertexPNT v2;
	//glm::vec3 centroid;
	glm::vec3 centroid;
	Material* mat_ptr;
};
