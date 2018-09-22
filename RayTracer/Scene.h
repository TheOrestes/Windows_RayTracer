#pragma once

#include <vector>
#include "Hitable.h"

class Scene
{
public:
	
	static Scene& getInstance()
	{
		static Scene instance;
		return instance;
	}

	
	bool Trace(const Ray& r, float tmin, float tmax, HitRecord& rec);

private:
	Scene();
	void InitScene();
	void InitRandomScene();

	std::vector<Hitable*> vecHitables;
};