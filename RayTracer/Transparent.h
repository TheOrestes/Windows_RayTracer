#pragma once

#include "Ray.h"
#include "Hitable.h"
#include "Helper.h"
#include "Material.h"
#include "Texture.h"

class Transparent : public Material
{
public:
	Transparent(Texture* _albedo, float ref_idx, float ref_roughness = 0.0f, float col_absorption = 0.0f)
		:	m_pTexAlbedo(_albedo),
			m_fRefractionIndex(ref_idx),
			m_fRefractionRoughness(ref_roughness),
			m_fColorAbsorption(col_absorption),
			Material(false, MaterialType::TRANSPARENT) {}
	
	virtual bool		Scatter(const Ray& r_in, const HitRecord& rec, int& rayCount, glm::vec3& outColor, Ray& scattered, float& pdf) const;

private:

	glm::vec3			BRDF(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const;

	Texture*			m_pTexAlbedo;
	float				m_fRefractionIndex;
	float				m_fRefractionRoughness;
	float				m_fColorAbsorption;

	uint32_t			m_uiMatID;
};