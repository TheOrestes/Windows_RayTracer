#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Texture.h"

class Emissive : public Material
{
public:
	Emissive(Texture* _emission) : Emission(_emission), m_bIsLightSource(true) {}

	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd, float& pdf) const
	{
		scatterd = Ray(rec.P, rec.P);
		pdf = 1.0f;
		return false;
	}

	virtual glm::vec3 Emitted(const Ray& r_in, const HitRecord& rec) const
	{
		if (glm::dot(-rec.N, glm::normalize(r_in.direction)) > 0.0f)
			return Emission->value(rec.uv);
		else
			return glm::vec3(0.0f);
	}

private:
	Texture*	Emission;

public:
	bool		m_bIsLightSource;
};
