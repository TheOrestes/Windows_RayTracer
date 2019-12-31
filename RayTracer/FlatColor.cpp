
#include "FlatColor.h"

bool FlatColor::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& attenuation, Ray& scatterd) const
{
	// Ray origin & Direction both are same, which means no further bounces
	// and processing of rays, useful for flat colors...!
	scatterd = Ray(rec.P, rec.P);

	attenuation = Albedo->value(rec.uv);
	return true;
}