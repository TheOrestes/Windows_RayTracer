
#include "Transparent.h"
#include "Helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Transparent::Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor , Ray& scattered) const
{
	glm::vec3 outward_normal;
	glm::vec3 ray_direction = r_in.direction;

	glm::vec3 reflected = Helper::Reflect(ray_direction, rec.N);
	float ni_over_nt;
	outColor = Albedo->value(rec.uv);

	glm::vec3 refracted = glm::vec3(0.0f, 0.0f, 0.0f);
	float reflect_prob;
	float cosine;

	// When ray shoots through object back into vacuum,
	// ni_over_nt = refr_idx, surface normal has to be inverted!
	if (glm::dot(ray_direction, rec.N) > 0)
	{
		outward_normal = -rec.N;
		ni_over_nt = refr_index;
		cosine = refr_index * glm::dot(ray_direction, rec.N) / glm::length(ray_direction);
	}
	// When ray shoots into the object, 
	// ni_over_nt = 1 / refr_idx
	else
	{
		outward_normal = rec.N;
		ni_over_nt = 1 / refr_index;
		cosine = -glm::dot(ray_direction, rec.N) / glm::length(ray_direction);
	}

	if (Helper::Refract(ray_direction, outward_normal, ni_over_nt, refracted))
	{
		reflect_prob = Helper::schlick(cosine, refr_index);
	}
	else
	{
		reflect_prob = 1.0f;
	}

	// this logic is not clear? 
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
		scattered = Ray(rec.P, refracted);
	}

	++rayCount;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Transparent::PDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	return 1.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Transparent::BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const
{
	return glm::vec3(1);
}

