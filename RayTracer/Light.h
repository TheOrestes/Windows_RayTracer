#pragma once

#include "glm/glm.hpp"
#include "Helper.h"
#include "Hitable.h"
#include "Sphere.h"
#include "TriangleMesh.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
enum LightType
{
	AMBIENT,
	DIRECTIONAL,
	POINT,
	SPHERE,
	AREA,
	MESH
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class Light
{
public:
	virtual glm::vec3 SampleLi(const HitRecord& rec, float* pdf) = 0;
	virtual glm::vec3 GetColor() = 0;

	LightType m_eLightType;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class AmbientLight : public Light
{
public:
	AmbientLight(glm::vec3 lightColor, float intensity)
		: m_fIntensity(intensity),
		m_colLight(lightColor) { m_eLightType = LightType::AMBIENT;	}

	glm::vec3			SampleLi(const HitRecord& rec, float* pdf) { *pdf = 1.0f;  return glm::vec3(0); }
	virtual glm::vec3	GetColor() { return m_fIntensity * m_colLight; }

public:
	float				m_fIntensity;
	glm::vec3			m_colLight;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class DirectionalLight : public Light
{
public:
	DirectionalLight(glm::vec3 lightDir, glm::vec3 lightColor, float intensity)
		: m_fIntensity(intensity),
		m_colLight(lightColor),
		m_vecDirection(lightDir) { m_eLightType = LightType::DIRECTIONAL;	}

	glm::vec3			SampleLi(const HitRecord& rec, float* pdf) { *pdf = 1.0f;  return m_vecDirection; }
	virtual glm::vec3	GetColor() { return m_fIntensity * m_colLight; }

public:
	float				m_fIntensity;
	glm::vec3			m_colLight;
	glm::vec3			m_vecDirection;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class PointLight : public Light
{
public:
	PointLight(glm::vec3 lightPos, glm::vec3 lightColor, float intensity)
		: m_fIntensity(intensity),
		m_colLight(lightColor),
		m_vecPosition(lightPos) { m_eLightType = LightType::POINT; }

	virtual glm::vec3 SampleLi(const HitRecord& rec, float* pdf) { *pdf = 1.0f;  return glm::normalize(m_vecPosition - rec.P); }
	virtual glm::vec3 GetColor() { return m_fIntensity * m_colLight; }

public:
	float		m_fIntensity;
	glm::vec3	m_colLight;
	glm::vec3	m_vecPosition;
};





