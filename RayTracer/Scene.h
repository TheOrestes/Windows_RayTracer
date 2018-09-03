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

	void InitScene();
	bool Trace(const Ray& r, float tmin, float tmax, HitRecord& rec);

private:
	Scene();

	std::vector<Hitable*> vecHitables;
};