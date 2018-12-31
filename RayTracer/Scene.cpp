
#include "Scene.h"
#include "Camera.h"
#include "Sphere.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Transparent.h"
#include "Texture.h"
#include "Triangle.h"
#include "TriangleMesh.h"

Scene::Scene()
{
	vecHitables.clear();
	InitScene();
	//InitRandomScene();
}

Scene::~Scene()
{
	vecHitables.clear();
}

void Scene::InitScene()
{
	glm::vec3 center0(-3.0f, 0.15f, 0);
	glm::vec3 albedo0(1, 0, 0);
	Material* pMatSphere0 = new Metal(new ConstantTexture(glm::vec3(1.0, 0.3, 0.0)), 0);
	Sphere* pSphere0 = new Sphere(center0, 0.8f, pMatSphere0);

	// Sphere2
	glm::vec3 center1(0, -100.5, 0);
	glm::vec3 albedo1(0.2);
	Material* pMatSphere1 = new Lambertian(new ConstantTexture(albedo1));
	Sphere* pSphere1 = new Sphere(center1, 100.0f, pMatSphere1);

	Sphere* pSphere2 = new Sphere(glm::vec3(-1.0, 0.0f, 1.5f), 0.5f, new Transparent(1.5f));
	Sphere* pSphere3 = new Sphere(glm::vec3(-4.0f, 0.4f, 0.0f), 1.0f, new Metal(new ConstantTexture(glm::vec3(1.0, 0.1, 0.0)), 0));
	Sphere* pSphere4 = new Sphere(glm::vec3(2.5f, 0.0f, 0), 0.5, new Lambertian(new ImageTexture("models/earth.jpg")));
	//Triangle* pTriangle0  = new Triangle(glm::vec3(-2.0f, 0.0f, -1.0f), glm::vec3(2.0f, 0.0f, -1.0f), glm::vec3(0.0f, 2.0f, -1.0f), new Metal(new ConstantTexture(glm::vec3(0.0, 1.0f, 0.0f)), 0.5f));
	Texture* baseTexture = new ImageTexture("models/car.jpg");
	Material* pMatMesh = new Lambertian(baseTexture);
	//TriangleMesh* pMesh0 = new TriangleMesh("models/barb1.fbx", pMatMesh);
	TriangleMesh* pMesh0 = new TriangleMesh("models/car.fbx", pMatMesh);

	//vecHitables.push_back(pSphere0);
	vecHitables.push_back(pSphere1);
	//vecHitables.push_back(pSphere2);
	vecHitables.push_back(pSphere3);
	vecHitables.push_back(pSphere4);
	//vecHitables.push_back(pTriangle0);
	vecHitables.push_back(pMesh0);
}

void Scene::InitRandomScene()
{
	Sphere* pSphere0 = new Sphere(glm::vec3(0, -1000, 0), 1000, new Lambertian(new ConstantTexture (glm::vec3(0.5, 0.5, 0.5))));
	vecHitables.push_back(pSphere0);

	int i = 1;

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float choose_mat = Helper::GetRandom01();

			glm::vec3 center(a + 0.9f * Helper::GetRandom01(), 0.2, b + 0.9 * Helper::GetRandom01());
			if (glm::length((center - glm::vec3(4, 0.2, 0))) > 0.9f)
			{
				if (choose_mat < 0.8f)
				{
					// diffuse
					Sphere* temp = new Sphere(center, 0.2f, new Lambertian(new ConstantTexture(glm::vec3(Helper::GetRandom01() * Helper::GetRandom01(), Helper::GetRandom01() * Helper::GetRandom01(), Helper::GetRandom01() * Helper::GetRandom01()))));
					vecHitables.push_back(temp);
				}
				else if (choose_mat < 0.95f)
				{
					// Metal
					Sphere* temp = new Sphere(center, 0.2f, new Metal(new ConstantTexture(glm::vec3(0.5f*(1 + Helper::GetRandom01()), 0.5f*(1 + Helper::GetRandom01()), 0.5f*(1 + Helper::GetRandom01()))), Helper::GetRandom01()));
					vecHitables.push_back(temp);
				}
				else
				{
					// glass
					Sphere* temp = new Sphere(center, 0.2f, new Transparent(1.5f));
					vecHitables.push_back(temp);
				}
			}
		}
	}

	Sphere* pSphere1 = new Sphere(glm::vec3(0, 1, 0), 1.0f, new Transparent(1.5f));
	Sphere* pSphere2 = new Sphere(glm::vec3(-4, 1, 0), 1.0f, new Lambertian(new ConstantTexture(glm::vec3(0.4f, 0.2f, 0.1f))));
	Sphere* pSphere3 = new Sphere(glm::vec3(4, 1, 0), 1.0f, new Metal(new ConstantTexture(glm::vec3(0.7f, 0.6f, 0.5f)), 0.0f));

	vecHitables.push_back(pSphere1);
	vecHitables.push_back(pSphere2);
	vecHitables.push_back(pSphere3);
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

