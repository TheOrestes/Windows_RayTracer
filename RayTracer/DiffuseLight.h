#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Texture.h"

class DiffuseLight : public Material
{
public:
	DiffuseLight(Texture* _emission)
	{
		Emission = _emission;
	}

	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& attenuation, Ray& scatterd) const
	{
		return false;
	}

	virtual glm::vec3 Emitted(const glm::vec2& uv) const
	{
		return Emission->value(uv);
	}

private:
	Texture* Emission;
};
