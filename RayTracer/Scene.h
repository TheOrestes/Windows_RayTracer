#pragma once

#include <vector>
#include "Hitable.h"

class Camera;

class Scene
{
public:
	~Scene();
	
	static Scene& getInstance()
	{
		static Scene instance;
		return instance;
	}

	bool Trace(const Ray& r, int& rayCount, float tmin, float tmax, HitRecord& rec);

private:
	Scene();
	void InitScene();
	void InitRandomScene();
	
	std::vector<Hitable*> vecHitables;
};