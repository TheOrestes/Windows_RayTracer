
#include "Lambertian.h"
#include "Helper.h"
#include "Scene.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Lambertian::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd, float& pdf) const
{
	glm::vec3 direction = Helper::CosineWeightedHemisphereSampling(glm::normalize(rec.N));

	glm::vec3 target = rec.P + direction;
	Ray brdfSampleRay = Ray(rec.P, target - rec.P);

	++rayCount;

	float NdotWi = glm::dot(glm::normalize(brdfSampleRay.direction), rec.N);

	glm::vec3 albedo = Albedo->value(rec.uv);

	// Lambert BRDF = rho * (INV_PI)
	glm::vec3 brdf = BRDF(r_in, rec, brdfSampleRay);
	
	scatterd = brdfSampleRay;

	// material pdf
	float materialPDF = glm::dot(brdfSampleRay.direction, rec.N) * INV_PI;

	// Mixture PDF
	pdf = materialPDF;
	//!---- PDF

	outColor = albedo * brdf * NdotWi;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Lambertian::BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	return glm::vec3(INV_PI);
}
