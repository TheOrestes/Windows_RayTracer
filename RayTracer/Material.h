#pragma once

#include "Ray.h"
#include "Hitable.h"

class Material
{
public:
	virtual bool		Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& albedo, Ray& scattered) const = 0;
	virtual glm::vec3	Emitted(const Ray& r_in, const HitRecord& rec) const { return glm::vec3(0); }

	virtual float		PDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const = 0;

	bool				m_bIsLightSource;
};
