
#include "Scene.h"
#include "Camera.h"
#include "Sphere.h"
#include "FlatColor.h"
#include "Lambertian.h"
#include "DiffuseLight.h"
#include "Metal.h"
#include "Transparent.h"
#include "Texture.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "Camera.h"
#include "../Profiler.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
Scene::Scene()
{
	m_colMiss = glm::vec4(0.5f);
	vecHitables.clear();
	m_pCamera = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Scene::~Scene()
{
	vecHitables.clear();
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitScene(float screenWidth, float screenHeight)
{
	// Initialize Camera first...!!!
	glm::vec3 cameraPosition = glm::vec3(5.0f, 2.5f, 5.0f);
	glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	m_pCamera = new Camera();
	m_pCamera->InitCamera(cameraPosition, cameraLookAt, screenWidth, screenHeight);

	// Sphere Ground
	glm::vec3 center1(0.0f, -100.5f, 0.0f);
	glm::vec3 albedo1(0.2f, 0.2f, 0.2f);
	Material* pMatSphereGround = new Lambertian(new ConstantTexture(albedo1));
	Sphere* pSphereGround = new Sphere(center1, 100.0f, pMatSphereGround);

	//Sphere* pSphereGlass1 = new Sphere(glm::vec3(-4.0f, 0.4f, 0.0f), 1.0f, new Transparent(1.3f));
	//Sphere* pSphereMetal = new Sphere(glm::vec3(0.0f, 0.7f, -3.5f), 1.4f, new Metal(new ConstantTexture(glm::vec3(1.0f, 0.1f, 0.0f)), 0.1f));
	//Sphere* pSphereLight = new Sphere(glm::vec3(-1.5f, 0.5f, 1.25f), 1.0f, new DiffuseLight(new ConstantTexture(glm::vec3(1.0f, 1.0f, 1.0f))));
	//Sphere* pSphereEarth = new Sphere(glm::vec3(2.5f, 0.0f, 0.0f), 0.5, new Lambertian(new ImageTexture("models/earth.jpg")));
	
	//MeshInfo barbInfo;
	//barbInfo.filePath = "models/barb1.fbx";
	//barbInfo.isLightSource = false;
	//barbInfo.leafSize = 512;
	//TriangleMesh* pMesh0 = new TriangleMesh(barbInfo);

	MeshInfo cubePhongInfo;
	cubePhongInfo.filePath = "models/CubePhong.fbx";
	cubePhongInfo.matInfo.albedoColor = glm::vec4(0, 1, 0, 1);
	cubePhongInfo.isLightSource = false;
	cubePhongInfo.leafSize = 12;
	TriangleMesh* pCubePhong = new TriangleMesh(cubePhongInfo);

	//Profiler::getInstance().WriteToProfiler("Triangle Count:", pMesh0->GetTriangleCount());

	vecHitables.push_back(pSphereGround);
	//vecHitables.push_back(pSphereGlass1);
	//vecHitables.push_back(pSphereMetal);
	//vecHitables.push_back(pSphereEarth);
	//vecHitables.push_back(pSphereLight);
	//vecHitables.push_back(pMesh0);
	vecHitables.push_back(pCubePhong);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitCornellScene(float screenWidth, float screenHeight)
{
	// Initialize Camera first...!!!
	glm::vec3 cameraPosition = glm::vec3(0.0f, 2.5f, 8.5f);
	glm::vec3 cameraLookAt = glm::vec3(0.0f, 2.5f, 0.0f);
	m_pCamera = new Camera();
	m_pCamera->InitCamera(cameraPosition, cameraLookAt, screenWidth, screenHeight);

	// Override miss color to black
	m_colMiss = glm::vec4(0.0f);
	
	Sphere* pSphereLight = new Sphere(glm::vec3(0.0f, 1.0f, 1.0f), 1.0f, new DiffuseLight(new ConstantTexture(glm::vec3(1.0f, 1.0f, 1.0f))));

	// Room Mesh
	MeshInfo roomInfo;
	roomInfo.filePath = "models/Cornell.fbx";
	roomInfo.isLightSource = false;
	roomInfo.leafSize = 10;
	TriangleMesh* pRoom = new TriangleMesh(roomInfo);

	// Cube 1
	MeshInfo cubeLeftInfo;
	cubeLeftInfo.filePath = "models/CubePhong.fbx";
	cubeLeftInfo.isLightSource = false;
	cubeLeftInfo.leafSize = 12;
	cubeLeftInfo.position = glm::vec3(1.0f, 1.8f, -1.35f);
	cubeLeftInfo.rotationAxis = glm::vec3(0, 1, 0);
	cubeLeftInfo.rotationAngle = 9.0f;
	cubeLeftInfo.scale = glm::vec3(1.6f, 3.6f, 1.6f);
	cubeLeftInfo.matInfo.albedoColor = glm::vec4(1, 1, 1, 1);
	cubeLeftInfo.matInfo.roughness = 0.2f;
	TriangleMesh* pLeftCube = new TriangleMesh(cubeLeftInfo);

	// Cube 2
	MeshInfo cubeRightInfo;
	cubeRightInfo.filePath = "models/Cube.fbx";
	cubeRightInfo.isLightSource = false;
	cubeRightInfo.leafSize = 10;
	cubeRightInfo.position = glm::vec3(-0.9f, 0.9f, 1.0f);
	cubeRightInfo.rotationAxis = glm::vec3(0, 1, 0);
	cubeRightInfo.rotationAngle = -6.0f;
	cubeRightInfo.scale = glm::vec3(1.8f);
	cubeRightInfo.matInfo.albedoColor = glm::vec4(0, 0, 1, 1);
	TriangleMesh* pRightCube = new TriangleMesh(cubeRightInfo);

	// Light Quad
	MeshInfo lightInfo;
	lightInfo.filePath = "models/Quad.fbx";
	lightInfo.isLightSource = true;
	lightInfo.leafSize = 2;
	lightInfo.position = glm::vec3(0, 4.99f, 0.5f);
	lightInfo.scale = glm::vec3(2);
	TriangleMesh* pLight = new TriangleMesh(lightInfo);

	vecHitables.push_back(pLight);
	vecHitables.push_back(pRoom);
	vecHitables.push_back(pLeftCube);
	vecHitables.push_back(pRightCube);

	Profiler::getInstance().WriteToProfiler("Triangle Count:", pRoom->GetTriangleCount() + pLight->GetTriangleCount() + pLeftCube->GetTriangleCount() + pRightCube->GetTriangleCount());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitRandomScene(float screenWidth, float screenHeight)
{
	// Initialize Camera first...!!!
	glm::vec3 cameraPosition = glm::vec3(5.0f, 2.5f, 5.0f);
	glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	m_pCamera = new Camera();
	m_pCamera->InitCamera(cameraPosition, cameraLookAt, screenWidth, screenHeight);

	Sphere* pSphere0 = new Sphere(glm::vec3(0, -1000.0f, 0), 1000, new Lambertian(new ConstantTexture (glm::vec3(0.5, 0.5, 0.5))));
	vecHitables.push_back(pSphere0);

	int i = 1;

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float choose_mat = Helper::GetRandom01();

			glm::vec3 center(a + 0.9f * Helper::GetRandom01(), 0.2f, b + 0.9f * Helper::GetRandom01());
			if (glm::length(center - glm::vec3(4.0f, 0.2f, 0.0f)) > 0.9f)
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

	Sphere* pSphere1 = new Sphere(glm::vec3(0.f, 1.f, 0.f), 1.0f, new Transparent(1.5f));
	Sphere* pSphere2 = new Sphere(glm::vec3(-4.f, 1.f, 0.f), 1.0f, new Lambertian(new ConstantTexture(glm::vec3(0.4f, 0.2f, 0.1f))));
	Sphere* pSphere3 = new Sphere(glm::vec3(4.f, 1.f, 0.f), 1.0f, new Metal(new ConstantTexture(glm::vec3(0.7f, 0.6f, 0.5f)), 0.0f));

	vecHitables.push_back(pSphere1);
	vecHitables.push_back(pSphere2);
	vecHitables.push_back(pSphere3);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Scene::Trace(const Ray& r, int& rayCount, float tmin, float tmax, HitRecord& rec)
{
	++rayCount;

	bool hit_anything = false;
	HitRecord temp_rec;
	float closest_so_far = tmax;

	for (int i = 0; i < vecHitables.size(); i++)
	{
		if (vecHitables[i]->hit(r, tmin, closest_so_far, temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
		else
		{
			// This is needed for Profile information
			// If ray doesn't hit anything, still it could have done BBox query or
			// Traiangle Query which needs to be accumulated..!
			rec = temp_rec;
		}
	}

	return hit_anything;
}

