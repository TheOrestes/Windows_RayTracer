#pragma once

#include <vector>
#include "Hitable.h"

class Camera;

class Scene
{
public:
	Scene();
	~Scene();

	bool Trace(const Ray& r, int& rayCount, float tmin, float tmax, HitRecord& rec);

	void InitSphereScene(float screenWidth, float screenHeight);
	void InitRefractionScene(float screenWidth, float screenHeight);
	void InitTigerScene(float screenWidth, float screenHeight);
	void InitCornellScene(float screenWidth, float screenHeight);
	void InitTowerScene(float screenWidth, float screenHeight);
	void InitRandomScene(float screenWidth, float screenHeight);

	inline Camera* getCamera() { if(m_pCamera) return m_pCamera; }
	
	glm::vec4	CalculateMissColor(glm::vec3 rayDirection);

private:	
	glm::vec4			  m_colMiss;
	Camera*				  m_pCamera;
	std::vector<Hitable*> vecHitables;

};