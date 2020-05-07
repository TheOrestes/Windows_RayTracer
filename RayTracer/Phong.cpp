
#include "Phong.h"
#include "Helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Phong::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd) const
{
	glm::vec3 PerfectReflDir = glm::normalize(Helper::Reflect(r_in.direction, rec.N));

	glm::vec3 direction = Helper::ModifiedPhongImportanceSampling(rec.N, PerfectReflDir, Ks, SpecularPower);
	scatterd = Ray(rec.P, glm::normalize(direction));

	++rayCount;

	glm::vec3 brdf = BRDF(r_in, rec, scatterd);
	outColor = brdf * Albedo->value(rec.uv);

	bool flag = (glm::dot(scatterd.direction, rec.N) > 0);
	return flag;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Phong::PDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	// Lambertian PDF
	float lambertPDF = Kd * INV_PI;
	
	// Specular PDF
	glm::vec3 PerfectReflDir = glm::normalize(Helper::Reflect(r_in.direction, rec.N));
	float alpha = glm::clamp(glm::dot(scattered.direction, PerfectReflDir), 0.0f, PI_OVER_TWO);
	float specularPDF = Ks * (SpecularPower + 1) / TWO_PI * powf(alpha, SpecularPower);

	return glm::clamp(lambertPDF + specularPDF, 0.0f, 1.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Phong::BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	// Diffuse BRDF
	float diffuseBRDF = Kd * INV_PI; 

	// Specular BRDF
	glm::vec3 PerfectReflDir = glm::normalize(Helper::Reflect(r_in.direction, rec.N));
	float alpha = glm::dot(scattered.direction, PerfectReflDir);

	float specBRDF = Ks * powf(alpha, SpecularPower) * ((alpha + 2) / TWO_PI);

	return glm::vec3(diffuseBRDF + specBRDF);
}
