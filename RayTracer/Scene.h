#pragma once

#include <vector>
#include "Hitable.h"
#include "Light.h"

class Camera;

class Scene
{
public:
	static Scene& getInstance()
	{
		static Scene inst;
		return inst;
	}

	~Scene();

	bool					Trace(const Ray& r, int& rayCount, float tmin, float tmax, HitRecord& rec);

	void					InitSphereScene(float screenWidth, float screenHeight);
	void					InitRefractionScene(float screenWidth, float screenHeight);
	void					InitTigerScene(float screenWidth, float screenHeight);
	void					InitCornellScene(float screenWidth, float screenHeight);
	void					InitTowerScene(float screenWidth, float screenHeight);
	void					InitRandomScene(float screenWidth, float screenHeight);
	
	glm::vec4				CalculateMissColor(glm::vec3 rayDirection);

private:	
	Scene();

	glm::vec4				m_colMiss;
	std::vector<Hitable*>	m_vecHitables;

public:
	std::vector<Light*>		m_vecLights;
};