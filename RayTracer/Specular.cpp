
#include "Specular.h"
#include "Helper.h"
#include "Scene.h"
#include <map>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Specular::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scattered, float& pdf) const
{
	glm::vec3 PerfectReflDir = glm::normalize(Helper::Reflect(r_in.direction, rec.N));

	glm::vec3 direction = Helper::ModifiedPhongImportanceSampling(rec.N, PerfectReflDir, Ks, SpecularPower);
	scattered = Ray(rec.P, glm::normalize(direction));

	++rayCount;

	glm::vec3 brdf = BRDF(r_in, rec, scattered);
	outColor = brdf * Albedo->value(rec.uv);

	//!---- PDF
	// Lambertian PDF
	float NdotWi = glm::clamp(glm::dot(r_in.direction, rec.N), 0.0f, 1.0f);
	float lambertPDF = Kd * INV_PI;

	// Specular PDF
	float alpha = glm::clamp(glm::dot(scattered.direction, PerfectReflDir), 0.0f, PI_OVER_TWO);
	float specularPDF = Ks * (SpecularPower + 1) / TWO_PI * powf(alpha, SpecularPower);

	pdf = glm::clamp(lambertPDF + specularPDF, 0.0f, 1.0f);
	//!---- PDF

	bool flag = (glm::dot(scattered.direction, rec.N) > 0);
	return flag;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Specular::BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	// Diffuse BRDF
	float diffuseBRDF = Kd * INV_PI; 

	// Specular BRDF
	glm::vec3 PerfectReflDir = glm::normalize(Helper::Reflect(r_in.direction, rec.N));
	float alpha = glm::dot(scattered.direction, PerfectReflDir);

	float D; float pdf;
	NormalDistributionTerm_GGX(r_in, rec, scattered, D, pdf);

	float F = FresnelTerm();
	float G = GeometricAttenuationTerm();

	float Nr = F * D * G;
	float Dr = 4.0f * (glm::dot(r_in.direction, rec.N)) * (glm::dot(scattered.direction, rec.N));

	float specBRDF = Nr / Dr;

	return glm::vec3(diffuseBRDF + specBRDF);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Specular::NormalDistributionTerm_GGX(const Ray& r_in, const HitRecord& rec, const Ray& scattered, float& D, float& pdf) const
{
	float alphaSqr = Roughness * Roughness;

	//!---- Light Sample Direction Vector
	std::map<Hitable*, HitableType>::iterator iter = Scene::getInstance().m_mapHitables.begin();
	std::map<Hitable*, HitableType>::iterator iterEnd = Scene::getInstance().m_mapHitables.end();

	glm::vec3 lightSampleVector;
	for (; iter != iterEnd; ++iter)
	{
		if (iter->second == HitableType::LIGHT)
		{
			lightSampleVector = iter->first->Sample(rec.P);
		}
	}

	glm::vec3 Half = glm::normalize(lightSampleVector + r_in.direction);
	float NdotH = glm::dot(rec.N, Half);
	float cosNdotH = std::cosf(NdotH);
	float tanNdotH = std::tanf(NdotH);

	float Nr = alphaSqr;
	float Dr = PI * std::powf(cosNdotH, 4.0f) * std::powf((alphaSqr + tanNdotH * tanNdotH), 2.0f);
	D = (Nr / Dr);

	// Calculate PDF
	float NdotWi = glm::dot(rec.N, lightSampleVector);
	float HdotWi = glm::dot(Half, lightSampleVector);

	pdf = (Kd * INV_PI * NdotWi) + (Ks * D * NdotH / 4.0f * HdotWi);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Specular::NormalDistributionTerm_Beckmann(const Ray& r_in, const HitRecord& rec, const Ray& scattered, float& D, float& pdf) const
{
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Specular::NormalDistributionTerm_Phong(const Ray& r_in, const HitRecord& rec, const Ray& scattered, float& D, float& pdf) const
{
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Specular::GeometricAttenuationTerm(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	//!---- Light Sample Direction Vector
	std::map<Hitable*, HitableType>::iterator iter = Scene::getInstance().m_mapHitables.begin();
	std::map<Hitable*, HitableType>::iterator iterEnd = Scene::getInstance().m_mapHitables.end();

	glm::vec3 lightSampleVector;
	for (; iter != iterEnd; ++iter)
	{
		if (iter->second == HitableType::LIGHT)
		{
			lightSampleVector = iter->first->Sample(rec.P);
		}
	}

	float NdotV = glm::dot(rec.N, r_in.direction);
	float NdotL = glm::dot(rec.N, lightSampleVector);

	float alpha = Roughness + 1.0f;
	float k = (alpha * alpha) / 8.0f;
	
	float ggx2 = NdotV / (NdotV * (1 - k) + k);
	float ggx1 = NdotL / (NdotL * (1 - k) + k);

	return ggx1 * ggx2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Specular::FresnelTerm() const
{
	float F0 = 0.03f;
	return 
}
