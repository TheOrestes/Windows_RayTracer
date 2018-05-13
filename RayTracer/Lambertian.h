#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Helper.h"

class Lambertian : public Material
{
public:
	Lambertian(const Vector3& _albedo) : Albedo(_albedo) {}

	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scatterd) const
	{
		Vector3 target = rec.P + rec.N + Helper::RandomInUnitSphere();
		scatterd = Ray(rec.P, target - rec.P);
		attenuation = Albedo;
		return true;
	}

private:
	Vector3 Albedo;
};