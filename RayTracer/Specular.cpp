
#include "Specular.h"
#include "Helper.h"
#include "Scene.h"
#include <map>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Specular::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scattered, float& pdf) const
{
	glm::vec3 Half = Helper::GGX_ImportanceSampling(rec.N,  Ks, Roughness);
	glm::vec3 Reflection = glm::reflect(r_in.direction, Half);

	glm::vec3 target = rec.P + Reflection;
	scattered = Ray(rec.P, target - rec.P);

	//if (glm::dot(rec.N, Reflection) > 0.0f && glm::dot(-r_in.direction, Half) > 0.0f)
	//{
		// PDF
		float D = 0.0f;
		NormalDistributionTerm_GGX(r_in, rec, scattered.direction, Half, D, pdf);

		++rayCount;

		glm::vec3 brdf = BRDF(r_in, rec, scattered, Half);
		outColor = brdf * Albedo->value(rec.uv);
	//}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Specular::BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered, const glm::vec3& Half) const
{
	// Diffuse BRDF
	float diffuseBRDF = Kd * INV_PI; 

	float D; float pdf;
	NormalDistributionTerm_GGX(r_in, rec, scattered.direction, Half, D, pdf);
	float G = GeometricAttenuationTerm(r_in, rec, scattered.direction);
	float F = FresnelTerm(Half, -r_in.direction);
	
	float Nr = D * G * F;
	float Dr = 4.0f * (glm::clamp(glm::dot(-r_in.direction, rec.N), 0.0f, 1.0f) * glm::clamp(glm::dot(scattered.direction, rec.N), 0.0f, 1.0f));

	float specBRDF = Ks * (Nr / std::max(Dr, 0.001f));

	return glm::vec3(diffuseBRDF + specBRDF);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Specular::NormalDistributionTerm_GGX(const Ray& r_in, const HitRecord& rec, const glm::vec3& lightDir, const glm::vec3& Half, float& D, float& pdf) const
{
	float alphaSqr = std::max(Roughness * Roughness, 0.001f);
	float NdotH = glm::clamp(glm::dot(rec.N, Half), 0.0f, 1.0f);
	float LdotH = glm::clamp(glm::dot(lightDir, Half), 0.0f, 1.0f);
	float NdotL = glm::clamp(glm::dot(rec.N, lightDir), 0.0f, 1.0f);

	float d = ((NdotH * alphaSqr - NdotH) * NdotH + 1.0f);
	
	D = alphaSqr / (d * d * PI);
	pdf = (Kd * INV_PI * NdotL) + Ks * (D * NdotH / (4 * LdotH));
	/*float cosNdotH = std::cosf(NdotH);
	float tanNdotH = std::tanf(NdotH);

	float Nr = alphaSqr;
	float Dr = PI * std::powf(cosNdotH, 4.0f) * std::powf((alphaSqr + (tanNdotH * tanNdotH)), 2.0f);
	D = (Nr / std::max(Dr, 0.001f));*/


	// Calculate PDF
	float NdotWi = glm::dot(rec.N, lightDir);
	float HdotWi = glm::max(glm::dot(Half, lightDir), 0.001f);

	//pdf = 1.0f; //(Kd * INV_PI * NdotWi) + (Ks * D * NdotH / 4.0f * HdotWi);
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
float Specular::GeometricAttenuationTerm(const Ray& r_in, const HitRecord& rec, const glm::vec3& lightDir) const
{
	float alphaSqr = std::max(Roughness * Roughness, 0.001f);
	float k = alphaSqr / 2.0f;

	float NdotV = glm::clamp(dot(rec.N, -r_in.direction), 0.0f, 1.0f);
	float NdotL = glm::clamp(dot(rec.N, lightDir), 0.0f, 1.0f);

	float smith1 = NdotV / (NdotV * (1 - k) + k);
	float smith2 = NdotL / (NdotL * (1 - k) + k);

	return smith1 * smith2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Specular::FresnelTerm(const glm::vec3& Half, const glm::vec3& viewDir) const
{
	float F0 = 0.03f;
	float VdotH = glm::clamp(glm::dot(viewDir, Half), 0.0f, 1.0f);

	return F0 + (1.0f - F0) * powf(1.0f - VdotH, 5.0f);
}
