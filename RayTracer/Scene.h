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

	void InitScene(float screenWidth, float screenHeight);
	void InitCornellScene(float screenWidth, float screenHeight);
	void InitTowerScene(float screenWidth, float screenHeight);
	void InitRandomScene(float screenWidth, float screenHeight);

	inline Camera* getCamera() { if(m_pCamera) return m_pCamera; }
	inline glm::vec4 getMissColor() { return m_colMiss; }

private:	
	glm::vec4			  m_colMiss;
	Camera*				  m_pCamera;
	std::vector<Hitable*> vecHitables;

};