#pragma once

#include "Ray.h"
#include "Hitable.h"

enum class MaterialType
{
	EMISSIVE,
	LAMBERT,
	METAL,
	PHONG,
	SPECULAR,
	TRANSPARENT
};

class Material
{
public:
	virtual bool		Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& albedo, Ray& scattered, float& pdf) const = 0;
	virtual glm::vec3	Emitted(const Ray& r_in, const HitRecord& rec) const { return glm::vec3(0); }

	bool				m_bIsLightSource;
	MaterialType		m_eType;

	Material(bool isLight, MaterialType _type) : m_bIsLightSource(isLight), m_eType(_type) {}
};
