
#include "Metal.h"
#include "Helper.h"

bool Metal::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& attenuation, Ray& scatterd) const
{
	glm::vec3 target = glm::reflect(glm::normalize(r_in.GetRayDirection()), rec.N);
	scatterd = Ray(rec.P, target + fuzz * Helper::RandomInUnitSphere());
	++rayCount;

	attenuation = Albedo->value(rec.uv);
	return (glm::dot(scatterd.GetRayDirection(), rec.N) > 0);
}