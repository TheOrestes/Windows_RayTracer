
#include "Metal.h"
#include "Helper.h"

bool Metal::Scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scatterd) const
{
	Vector3 target = Helper::Reflect(unit_vector(r_in.GetRayDirection()), rec.N);
	scatterd = Ray(rec.P, target + fuzz * Helper::RandomInUnitSphere());
	attenuation = Albedo;
	return (dot(scatterd.GetRayDirection(), rec.N) > 0);
}