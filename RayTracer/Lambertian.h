#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Texture.h"

class Lambertian : public Material
{
public:
	Lambertian(Texture* _albedo) : Albedo(_albedo), m_bIsLightSource(false) {}

	virtual bool	Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd) const;
	virtual float	PDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const;

private:

	glm::vec3		BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const;
	Texture*		Albedo;

public:
	bool			m_bIsLightSource;
};