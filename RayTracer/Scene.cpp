
#include "Scene.h"
#include "Sphere.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Transparent.h"
#include "Triangle.h"

Scene::Scene()
{
	vecHitables.clear();
	InitScene();
}

void Scene::InitScene()
{
	Vector3 center0(0.0f, 0, -3);
	Vector3 albedo0(1, 0, 0);
	Material* pMatSphere0 = new Metal(albedo0, 0.2f);
	Sphere* pSphere0 = new Sphere(center0, 0.5, pMatSphere0);

	// Sphere2
	Vector3 center1(0, -100.5, 0);
	Vector3 albedo1(0.2, 0.2, 0.2);
	Material* pMatSphere1 = new Lambertian(albedo1);
	Sphere* pSphere1 = new Sphere(center1, 100.0f, pMatSphere1);

	Sphere* pSphere2 = new Sphere(Vector3(0, 0, 2), 0.5, new Lambertian(Vector3(1.0f, 0.0f, 0.0f)));
	Sphere* pSphere3 = new Sphere(Vector3(-1.05f, 0, 0), 0.5, new Metal(Vector3(1.0, 0.2, 0.0), 0));
	Sphere* pSphere4 = new Sphere(Vector3(1.05f, 0, 0), 0.5, new Transparent(1.5f));
	Triangle* pTriangle0  = new Triangle(Vector3(-2.0f, 0.0f, -1.0f), Vector3(2.0f, 0.0f, -1.0f), Vector3(0.0f, 2.0f, -1.0f), new Metal(Vector3(0.0, 1.0f, 0.0f), 0.1f));

	vecHitables.push_back(pSphere0);
	vecHitables.push_back(pSphere1);
	vecHitables.push_back(pSphere2);
	vecHitables.push_back(pSphere3);
	vecHitables.push_back(pSphere4);
	vecHitables.push_back(pTriangle0);
}

bool Scene::Trace(const Ray& r, float tmin, float tmax, HitRecord& rec)
{
	bool hit_anything = false;
	HitRecord temp_rec;
	double closest_so_far = tmax;

	for (int i = 0; i < vecHitables.size(); i++)
	{
		if (vecHitables[i]->hit(r, tmin, closest_so_far, temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}

