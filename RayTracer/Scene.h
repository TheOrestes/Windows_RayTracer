#pragma once

#include <map>
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
	void					InitMaterialTestScene(float screenWidth, float screenHeight);
	void					InitRandomScene(float screenWidth, float screenHeight);
	
	glm::vec3				CalculateMissColor(glm::vec3 rayDirection);

private:	
	Scene();
	glm::vec3				m_colMiss;

public:
	std::map<Hitable*, HitableType>	m_mapHitables;
	std::vector<Light*>		m_vecLights;
};