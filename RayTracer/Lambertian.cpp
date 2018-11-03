
#include "Lambertian.h"
#include "Helper.h"

bool Lambertian::Scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scatterd) const
{
	glm::vec3 target = rec.P + rec.N + Helper::RandomInUnitSphere();
	scatterd = Ray(rec.P, target - rec.P);
	attenuation = Albedo;
	return true;
}