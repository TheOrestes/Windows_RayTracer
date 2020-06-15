
#include "Phong.h"
#include "Helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Phong::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scattered, float& pdf) const
{
	glm::vec3 PerfectReflDir = glm::normalize(Helper::Reflect(r_in.direction, rec.N));

	glm::vec3 direction = Helper::ModifiedPhongImportanceSampling(rec.N, PerfectReflDir, Ks, SpecularPower);
	//glm::vec3 direction = Helper::PhongImportanceSampling(rec.N, PerfectReflDir, Ks, SpecularPower);

	glm::vec3 target = rec.P + direction;
	Ray brdfOutRay = Ray(rec.P, target - rec.P);

	++rayCount;

	glm::vec3 brdf = BRDF(r_in, rec, brdfOutRay);
	outColor = brdf * Albedo->value(rec.uv);

	//!----- PDF
	// Lambertian PDF
	float NdotWi = glm::dot(glm::normalize(brdfOutRay.direction), rec.N);
	float lambertPDF = Kd * NdotWi * INV_PI;

	// Specular PDF
	float alpha = glm::clamp(glm::dot(brdfOutRay.direction, PerfectReflDir), 0.001f, PI_OVER_TWO);
	float specularPDF = Ks * (SpecularPower + 1) / TWO_PI * powf(alpha, SpecularPower);

	pdf = glm::clamp(lambertPDF + specularPDF, 0.001f, 1.0f);
	//!----- PDF

	bool flag = (glm::dot(brdfOutRay.direction, rec.N) > 0);
	scattered = brdfOutRay;

	return flag;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Phong::BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	// Diffuse BRDF
	float diffuseBRDF = Kd * INV_PI; 

	// Specular BRDF
	glm::vec3 PerfectReflDir = glm::normalize(Helper::Reflect(r_in.direction, rec.N));
	float alpha = glm::max(glm::dot(scattered.direction, PerfectReflDir), 0.001f);

	float specBRDF = Ks * powf(alpha, SpecularPower) * ((alpha + 2) / TWO_PI);

	return glm::vec3(diffuseBRDF + specBRDF);
}
