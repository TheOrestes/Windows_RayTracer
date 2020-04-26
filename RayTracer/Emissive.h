#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Texture.h"

class Emissive : public Material
{
public:
	Emissive(Texture* _emission)
	{
		Emission = _emission;
	}

	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scatterd) const
	{
		return false;
	}

	virtual float PDF(const Ray& r_in, const HitRecord& rec, const Ray& scatterd) const
	{
		return 1.0f;
	}

	virtual glm::vec3 Emitted(const glm::vec2& uv) const
	{
		return Emission->value(uv);
	}

private:
	Texture* Emission;
};
