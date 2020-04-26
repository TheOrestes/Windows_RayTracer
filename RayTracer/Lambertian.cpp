
#include "Lambertian.h"
#include "Helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Lambertian::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd) const
{
	glm::vec3 target = rec.P + rec.N + Helper::RandomUnitVector();

	scatterd = Ray(rec.P, target - rec.P);
	++rayCount;

	float NdotWi = glm::dot(r_in.direction, rec.N);

	glm::vec3 albedo = Albedo->value(rec.uv);

	// Lambert BRDF = rho * (INV_PI)
	glm::vec3 brdf = BRDF(r_in, rec, scatterd);

	outColor = albedo * brdf * NdotWi;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Lambertian::PDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	float NdotWi = glm::dot(r_in.direction, rec.N);
	return NdotWi * INV_PI;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Lambertian::BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	return glm::vec3(INV_PI);
}
