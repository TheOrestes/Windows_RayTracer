
#include "Metal.h"
#include "Helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Metal::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd, float& pdf) const
{
	glm::vec3 target = glm::normalize(Helper::Reflect(r_in.direction, rec.N));
	scatterd = Ray(rec.P, target + fuzz * Helper::RandomInUnitSphere());
	++rayCount;

	outColor = Albedo->value(rec.uv);

	//!---- PDF
	pdf = 1.0f;
	//!---- PDF

	return (glm::dot(scatterd.direction, rec.N) > 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Metal::BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	return glm::vec3(1);
}
