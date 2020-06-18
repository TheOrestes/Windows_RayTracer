
#include "Transparent.h"
#include "Helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Transparent::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor , Ray& scattered, float& pdf) const
{
	glm::vec3 outward_normal;
	glm::vec3 ray_direction = r_in.direction;

	glm::vec3 reflected = Helper::Reflect(ray_direction, rec.N);
	float ni_over_nt;
	outColor = m_pTexAlbedo->value(rec.uv);

	glm::vec3 refracted = glm::vec3(0.0f, 0.0f, 0.0f);
	float reflect_prob;
	float cosine;

	// We are checking if ray in entering the surface of exiting the surface.
	// dot product will be > 0 when ray is entering a medium from air 
	// during this case, 
	// 1. refraction normal will be inverted!
	// 2. refraction index of medium will used
	if (glm::dot(ray_direction, rec.N) > 0)
	{
		outward_normal = -rec.N;
		ni_over_nt = m_fRefractionIndex;
		cosine = m_fRefractionIndex * glm::dot(ray_direction, rec.N) / glm::length(ray_direction);
	}
	else
	{
		// this is the scenario, when ray is exiting the medium back into air!
		// during this case,
		// 1. refraction normal will be the original normal
		// 2. refraction index will be inverted
		outward_normal = rec.N;
		ni_over_nt = 1 / m_fRefractionIndex;
		cosine = -glm::dot(ray_direction, rec.N) / glm::length(ray_direction);
	}

	// Here we will try to find refracted ray & the probability of reflection 
	// when ray refracts. Reflection probability is ONE when it's not refracting!
	if (Helper::Refract(ray_direction, outward_normal, ni_over_nt, refracted))
	{
		reflect_prob = Helper::schlick(cosine, m_fRefractionIndex);
	}
	else
	{
		reflect_prob = 1.0f;
	}

	// Both reflection and refraction of the light occur for dielectric material, but we can only 
	// pick 1 scattered ray for next iteration of ray tracing. Since we are shooting multiple rays 
	// per pixel (multi-sampling) and average the traced color as final pixel color, we can use the 
	// same idea to get the averaged result through both reflectiona and refraction.

	// Now we generate a random number between 0.0 and 1.0. If it’s smaller than reflective coefficient, 
	// the scattered ray is recorded as reflected; If it’s bigger than reflective coefficient, 
	// the scattered ray is recorded as refracted.
	if (Helper::GetRandom01() < reflect_prob)
	{
		scattered = Ray(rec.P, reflected);
	}
	else
	{
		scattered = Ray(rec.P, refracted + (m_fRefractionRoughness * m_fRefractionRoughness * Helper::RandomInUnitSphere()));

		glm::vec3 absorptionColor = outColor * glm::exp(-glm::vec3(10,0,0) * glm::length(rec.t));
		glm::vec3 originalColor = outColor;
		
		outColor = Helper::LerpVector(originalColor, absorptionColor, m_fColorAbsorption);
	}

	pdf = 1.0f;

	++rayCount;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Transparent::BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	return glm::vec3(1);
}

