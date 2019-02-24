#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Material.h"
#include "Texture.h"

class FlatColor : public Material
{
public:
	FlatColor(Texture* _albedo) : Albedo(_albedo) {}

	virtual bool Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& attenuation, Ray& scatterd) const;

private:
	Texture* Albedo;
};
