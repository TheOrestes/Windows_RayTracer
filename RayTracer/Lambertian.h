#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"

class Lambertian : public Material
{
public:
	Lambertian(const Vector3& _albedo) : Albedo(_albedo) {}

	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scatterd) const;

private:
	Vector3 Albedo;
};