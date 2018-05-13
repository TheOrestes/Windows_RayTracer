#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Helper.h"

class Metal : public Material
{
public:
	Metal (const Vector3& _albedo, float f) : Albedo(_albedo) 
	{
		if (f < 1)
			fuzz = f;
		else
			fuzz = 1;
	}

	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scatterd) const
	{
		Vector3 target = Helper::Reflect(unit_vector(r_in.GetRayDirection()), rec.N);
		scatterd = Ray(rec.P, target + fuzz * Helper::RandomInUnitSphere());
		attenuation = Albedo;
		return (dot(scatterd.GetRayDirection(), rec.N) > 0);
	}

private:
	Vector3 Albedo;
	float fuzz;
};
