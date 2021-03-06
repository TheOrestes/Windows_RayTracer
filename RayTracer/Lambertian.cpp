
#include "Lambertian.h"
#include "Helper.h"

bool Lambertian::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& attenuation, Ray& scatterd) const
{
	glm::vec3 target = rec.P + rec.N + Helper::RandomUnitVector();
	scatterd = Ray(rec.P, target - rec.P);
	++rayCount;

	attenuation = Albedo->value(rec.uv);
	return true;
}