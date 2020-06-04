
#include "Scene.h"
#include "Camera.h"
#include "Sphere.h"
#include "FlatColor.h"
#include "Lambertian.h"
#include "Phong.h"
#include "Specular.h"
#include "Emissive.h"
#include "Metal.h"
#include "Transparent.h"
#include "Texture.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "XYRect.h"
#include "XZRect.h"
#include "Camera.h"
#include "Light.h"
#include "../Main/Profiler.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
Scene::Scene()
{
	m_colMiss = glm::vec4(0.5f);
	m_mapHitables.clear();
	m_vecLights.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Scene::~Scene()
{
	m_mapHitables.clear();
	m_vecLights.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitSphereScene(float screenWidth, float screenHeight)
{
	// Initialize Camera first...!!!
	glm::vec3 cameraPosition = glm::vec3(0.0f, 1.5f, 5.0f);
	glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, 0.0f);

	Camera::getInstance().InitCamera(cameraPosition, cameraLookAt, screenWidth, screenHeight);

	// Override miss color to black
	m_colMiss = glm::vec4(0.0f);

	glm::vec4 redColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 greenColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec4 blueColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	// Sphere Ground
	glm::vec3 center1(0.0f, -200.5f, 0.0f);
	glm::vec3 matColor(1,1,0);
	Material* pMatSphereGround = new Lambertian(new ConstantTexture(glm::vec3(0.25f, 0.25f, 0.25f)));
	Sphere* pSphereGround = new Sphere(center1, 200.0f, pMatSphereGround);

	CheckeredTexture* checksTexture = new CheckeredTexture(glm::vec3(0.2f, 0.9f, 0.5f), glm::vec3(0.03f), 10.0f, 10.0f);
	glm::vec4 glassColor = glm::vec4(10, 10, 100, 1);

	glm::vec4 lightColor = glm::vec4(20.0f, 20.0f, 20.0f, 1.0f);
	XZRect* pRect = new XZRect(glm::vec3(0.0f, 2.0f, 0.0f), 2.0f, 2.0f, new Emissive(new ConstantTexture(lightColor)));

	Sphere* pSphereGlass1 = new Sphere(glm::vec3(0.0f, 0.15f, 1.0f), 0.3f, new Emissive(new ConstantTexture(glassColor)));
	Sphere* pSpherePhong = new Sphere(glm::vec3(-1.5f, 0.0f, 0.0f), 0.5f, new Lambertian(new ConstantTexture(matColor)));// , 512.0f, 1.0f));
	Sphere* pSphereMetal = new Sphere(glm::vec3(1.5f, 0.0f, 0.0f), 0.5f, new Metal(new ConstantTexture(greenColor), 0.0f));// , 512.0, 1.0f));
	//Sphere* pSphereLight = new Sphere(glm::vec3(-0.5f, 0.1f, 2.0f), 0.2f, new Emissive(new ConstantTexture(glm::vec3(10.0f, 10.0f, 10.0f))));
	Sphere* pSphereEarth = new Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, new Lambertian(new ImageTexture("models/earth.jpg")));

	//Profiler::getInstance().WriteToProfiler("Triangle Count:", pMesh0->GetTriangleCount());

	m_mapHitables.insert(std::make_pair(pSphereGround, HitableType::GEOMETRY));
	m_mapHitables.insert(std::make_pair(pRect, HitableType::LIGHT));
	m_mapHitables.insert(std::make_pair(pSpherePhong, HitableType::GEOMETRY));
	m_mapHitables.insert(std::make_pair(pSphereMetal, HitableType::GEOMETRY));
	m_mapHitables.insert(std::make_pair(pSphereEarth, HitableType::GEOMETRY));
	//m_mapHitables.push_back(pSphereLight);

	// Scene Lights...
	//AmbientLight* pAmbientLight = new AmbientLight(glm::vec3(0.5f), 1.0f);
	//DirectionalLight* pDirLight = new DirectionalLight(glm::vec3(0,1,0), glm::vec3(1.0f), 1.0f);
	//PointLight* pPointLight = new PointLight(glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 1.0f);
	//m_vecLights.push_back(pAmbientLight);
	//m_vecLights.push_back(pDirLight);
	//m_vecLights.push_back(pPointLight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitRefractionScene(float screenWidth, float screenHeight)
{
	// Initialize Camera first...!!!
	glm::vec3 cameraPosition = glm::vec3(0.0f, 3.5f, 7.0f);
	glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	
	Camera::getInstance().InitCamera(cameraPosition, cameraLookAt, screenWidth, screenHeight);

	// Override miss color to black
	m_colMiss = glm::vec4(0.78f, 0.88f, 1.0f, 1.0f);

	// Sphere Ground
	glm::vec3 center1(0.0f, -200.5f, 0.0f);
	glm::vec3 albedo1(0.2f, 0.2f, 0.2f);
	Material* pMatSphereGround = new Lambertian(new ConstantTexture(glm::vec3(0.25f, 0.25f, 0.25f)));
	Sphere* pSphereGround = new Sphere(center1, 200.0f, pMatSphereGround);

	CheckeredTexture* checksTexture = new CheckeredTexture(glm::vec3(0.2f, 0.9f, 0.5f), glm::vec3(0.03f), 10.0f, 10.0f);
	glm::vec4 glassColor = glm::vec4(0.5, 1, 0.25, 1);

	Sphere* pSphereGlass1 = new Sphere(glm::vec3(0.0f, 0.75f, 0.0f), 1.5f, new Transparent(new ConstantTexture(glassColor), 1.5f));
	Sphere* pSphereMetal = new Sphere(glm::vec3(0.75f, 0.5f, -5.0f), 1.0f, new Metal(checksTexture, 0.1f));
	
	//Profiler::getInstance().WriteToProfiler("Triangle Count:", pMesh0->GetTriangleCount());

	m_mapHitables.insert(std::make_pair(pSphereGround, HitableType::GEOMETRY));
	m_mapHitables.insert(std::make_pair(pSphereGlass1, HitableType::GEOMETRY));
	m_mapHitables.insert(std::make_pair(pSphereMetal, HitableType::GEOMETRY));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitTigerScene(float screenWidth, float screenHeight)
{
	// Initialize Camera first...!!!
	glm::vec3 cameraPosition = glm::vec3(0.0f, 1.5f, 5.0f);
	glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	
	Camera::getInstance().InitCamera(cameraPosition, cameraLookAt, screenWidth, screenHeight);

	// Override miss color to black
	m_colMiss = glm::vec4(0.78f, 0.88f, 1.0f, 1.0f);

	// Sphere Ground
	glm::vec3 center1(0.0f, -200.5f, 0.0f);
	glm::vec3 albedo1(0.2f, 0.2f, 0.2f);
	Material* pMatSphereGround = new Lambertian(new CheckeredTexture(glm::vec3(0.2f, 0.9f, 0.5f), glm::vec3(0.03f), 30.0f, 600.0f));
	Sphere* pSphereGround = new Sphere(center1, 200.0f, pMatSphereGround);


	//Sphere* pSphereGlass1 = new Sphere(glm::vec3(-4.0f, 0.4f, 0.0f), 1.0f, new Transparent(1.3f));
	Sphere* pSphereMetal = new Sphere(glm::vec3(3.5f, 0.5f, 0.0f), 1.0f, new Metal(new ConstantTexture(glm::vec3(1.0f, 0.1f, 0.0f)), 0.1f));
	//Sphere* pSphereLight = new Sphere(glm::vec3(-1.5f, 0.5f, 1.25f), 1.0f, new Emissive(new ConstantTexture(glm::vec3(1.0f, 1.0f, 1.0f))));
	Sphere* pSphereEarth = new Sphere(glm::vec3(2.5f, 0.0f, 0.0f), 0.5, new Lambertian(new ImageTexture("models/earth.jpg")));
	
	//MeshInfo barbInfo;
	//barbInfo.filePath = "models/barb1.fbx";
	//barbInfo.isLightSource = false;
	//barbInfo.leafSize = 512;
	//TriangleMesh* pMesh0 = new TriangleMesh(barbInfo);

	// Glass Mesh
	MeshInfo glassTigerInfo;
	glassTigerInfo.filePath = "models/tigerTransparent.fbx";
	glassTigerInfo.leafSize = 512;
	glassTigerInfo.position = glm::vec3(-1.0f, -0.75f, 1.0f);
	glassTigerInfo.rotationAxis = glm::vec3(0, 1, 0);
	glassTigerInfo.rotationAngle = 200.0f;
	glassTigerInfo.scale = glm::vec3(0.75f);
	glassTigerInfo.matInfo.albedoColor = glm::vec4(0.3f, 0.8f, 1.0f, 1);
	glassTigerInfo.matInfo.refrIndex = 1.4f;
	TriangleMesh* pGlassTiger = new TriangleMesh(glassTigerInfo);

	// Light Quad
	MeshInfo lightInfo;
	lightInfo.filePath = "models/Quad.fbx";
	lightInfo.isLightSource = true;
	lightInfo.leafSize = 2;
	lightInfo.position = glm::vec3(0, 22.0f, 0.0f);
	lightInfo.scale = glm::vec3(200);
	lightInfo.matInfo.albedoColor = glm::vec4(glm::vec3(1.5f), 1);
	TriangleMesh* pLight = new TriangleMesh(lightInfo);
	
	// base Quad
	MeshInfo baseInfo;
	baseInfo.filePath = "models/QuadBase.fbx";
	baseInfo.isLightSource = false;
	baseInfo.leafSize = 2;
	baseInfo.position = glm::vec3(0.0f, -0.5f, 0.0f);
	baseInfo.rotationAxis = glm::vec3(0, 1, 0);
	baseInfo.rotationAngle = 0.0f;
	baseInfo.scale = glm::vec3(10.0f);
	//lightInfo.matInfo.albedoColor = glm::vec4(glm::vec3(1.5f), 1);
	TriangleMesh* pBase = new TriangleMesh(baseInfo);

	//Profiler::getInstance().WriteToProfiler("Triangle Count:", pMesh0->GetTriangleCount());

	//m_mapHitables.push_back(pSphereGround);
	//m_mapHitables.push_back(pSphereGlass1);
	//m_mapHitables.push_back(pSphereMetal);
	//m_mapHitables.push_back(pSphereEarth);
	//m_mapHitables.push_back(pSphereLight);
	//m_mapHitables.push_back(pBase);
	//m_mapHitables.push_back(pLight);
	//m_mapHitables.push_back(pGlassTiger);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitCornellScene(float screenWidth, float screenHeight)
{
	// Initialize Camera first...!!!
	glm::vec3 cameraPosition = glm::vec3(0.0f, 2.5f, 8.5f);
	glm::vec3 cameraLookAt = glm::vec3(0.0f, 2.5f, 0.0f);
	
	Camera::getInstance().InitCamera(cameraPosition, cameraLookAt, screenWidth, screenHeight);

	// Override miss color to black
	m_colMiss = glm::vec4(0.2f);
	
	glm::vec4 whiteColor = glm::vec4(1,1,1,1);
	Sphere* pPhongSphere = new Sphere(glm::vec3(1.0f, 0.6f, 0.5f), 0.6f, new Phong(new ConstantTexture(whiteColor), 512.0f, 1.0f));
	Sphere* pLambertSphere = new Sphere(glm::vec3(-1.0f, 0.6, 0.5f), 0.6f, new Lambertian(new ConstantTexture(whiteColor)));

	//glm::vec4 phongColor = glm::vec4(1, 1, 1, 1);
	//Sphere* pPhongSphere = new Sphere(glm::vec3(1.0f, 1.0f, 0.5f), 1.0f, new Phong(new ConstantTexture(phongColor), 512.0f, 1.0f));

	glm::vec4 lightColor = glm::vec4(5,5,5,0);
	Sphere* pLightSphere = new Sphere(glm::vec3(1.0f, 1.0f, 1.5f), 0.5f, new Emissive(new ConstantTexture(lightColor)));// , 512.0f, 0.0f));

	XZRect* pRect = new XZRect(glm::vec3(0.0f, 4.99f, 0.0f), 3.0f, 3.0f, new Emissive(new ConstantTexture(lightColor)));

	// Room Mesh
	MeshInfo roomInfo;
	roomInfo.filePath = "models/Cornell.fbx";
	roomInfo.isLightSource = false;
	roomInfo.leafSize = 10;
	TriangleMesh* pRoom = new TriangleMesh(roomInfo);

	// Cube Left Big
	MeshInfo cubeBigInfo;
	cubeBigInfo.filePath = "models/Cube.fbx";
	cubeBigInfo.isLightSource = false;
	cubeBigInfo.leafSize = 12;
	cubeBigInfo.position = glm::vec3(-1.0f, 1.5f, -1.1f);
	cubeBigInfo.rotationAxis = glm::vec3(0, 1, 0);
	cubeBigInfo.rotationAngle = 20.0f;
	cubeBigInfo.scale = glm::vec3(1.6f, 3.0f, 1.6f);
	//cubeLeftInfo.matInfo.albedoColor = glm::vec4(1,1,1,1);
	//cubeLeftInfo.matInfo.roughness = 0.5f;
	TriangleMesh* pBigCube = new TriangleMesh(cubeBigInfo);

	// Cube Right Small
	MeshInfo cubeSmallInfo;
	cubeSmallInfo.filePath = "models/Cube.fbx";
	cubeSmallInfo.isLightSource = false;
	cubeSmallInfo.leafSize = 12;
	cubeSmallInfo.position = glm::vec3(-0.8f, 0.7f, 1.5f);
	cubeSmallInfo.rotationAxis = glm::vec3(0, 1, 0);
	cubeSmallInfo.rotationAngle = 20.0f;
	cubeSmallInfo.scale = glm::vec3(1.4f, 1.4f, 1.4f);
	//cubeRightInfo.matInfo.albedoColor = glm::vec4(1,1,1,1);
	//cubeRightInfo.matInfo.roughness = 0.5f;
	TriangleMesh* pSmallCube = new TriangleMesh(cubeSmallInfo);

	// Light Quad
	MeshInfo lightInfo;
	lightInfo.filePath = "models/Quad.fbx";
	lightInfo.isLightSource = true;
	lightInfo.leafSize = 2;
	lightInfo.position = glm::vec3(0, 4.99f, 0.5f);
	lightInfo.scale = glm::vec3(1.2);
	lightInfo.matInfo.albedoColor = glm::vec4(glm::vec3(50.0f), 1);
	TriangleMesh* pLight = new TriangleMesh(lightInfo);

	// Glass Mesh
	//MeshInfo glassTigerInfo;
	//glassTigerInfo.filePath = "models/tigerTransparent.fbx";
	//glassTigerInfo.leafSize = 512;
	//glassTigerInfo.position = glm::vec3(-2.0f, 0.0f, 1.0f);
	//glassTigerInfo.rotationAxis = glm::vec3(0, 1, 0);
	//glassTigerInfo.rotationAngle = -45.0f;
	//glassTigerInfo.scale = glm::vec3(0.5f);
	//glassTigerInfo.matInfo.albedoColor = glm::vec4(1.0f, 1.0f, 0, 1);
	//glassTigerInfo.matInfo.refrIndex = 1.4f;
	//TriangleMesh* pGlassTiger = new TriangleMesh(glassTigerInfo);

	m_mapHitables.insert(std::make_pair(pRoom, HitableType::GEOMETRY));
	//m_mapHitables.insert(std::make_pair(pBigCube, HitableType::GEOMETRY));
	//m_mapHitables.insert(std::make_pair(pSmallCube, HitableType::GEOMETRY));
	//m_mapHitables.insert(std::make_pair(pLightSphere, HitableType::GEOMETRY));
	m_mapHitables.insert(std::make_pair(pPhongSphere, HitableType::GEOMETRY));
	m_mapHitables.insert(std::make_pair(pLambertSphere, HitableType::GEOMETRY));
	m_mapHitables.insert(std::make_pair(pRect, HitableType::LIGHT));
	//m_mapHitables.insert(std::make_pair(pLight, HitableType::LIGHT));
	//m_mapHitables.push_back(pSphereGlass);
	//m_mapHitables.push_back(pLightSphere);
	//m_mapHitables.push_back(pGlassTiger);

	Profiler::getInstance().WriteToProfiler("Triangle Count:", pRoom->GetTriangleCount() + pLight->GetTriangleCount() + pBigCube->GetTriangleCount());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Scene::InitRandomScene(float screenWidth, float screenHeight)
{
	// Initialize Camera first...!!!
	glm::vec3 cameraPosition = glm::vec3(5.0f, 2.5f, 5.0f);
	glm::vec3 cameraLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	
	Camera::getInstance().InitCamera(cameraPosition, cameraLookAt, screenWidth, screenHeight);

	Sphere* pSphere0 = new Sphere(glm::vec3(0, -1000.0f, 0), 1000, new Lambertian(new ConstantTexture (glm::vec3(0.5, 0.5, 0.5))));
	m_mapHitables.insert(std::make_pair(pSphere0, HitableType::GEOMETRY));

	int i = 1;
	glm::vec4 glassColor = glm::vec4(1, 1, 0, 1);

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
					m_mapHitables.insert(std::make_pair(temp, HitableType::GEOMETRY));
				}
				else if (choose_mat < 0.95f)
				{
					// Metal
					Sphere* temp = new Sphere(center, 0.2f, new Metal(new ConstantTexture(glm::vec3(0.5f*(1 + Helper::GetRandom01()), 0.5f*(1 + Helper::GetRandom01()), 0.5f*(1 + Helper::GetRandom01()))), Helper::GetRandom01()));
					m_mapHitables.insert(std::make_pair(temp, HitableType::GEOMETRY));
				}
				else
				{
					// glass
					Sphere* temp = new Sphere(center, 0.2f, new Transparent(new ConstantTexture(glassColor), 1.5f));
					m_mapHitables.insert(std::make_pair(temp, HitableType::GEOMETRY));
				}
			}
		}
	}

	Sphere* pSphere1 = new Sphere(glm::vec3(0.f, 1.f, 0.f), 1.0f, new Transparent(new ConstantTexture(glassColor), 1.5f));
	Sphere* pSphere2 = new Sphere(glm::vec3(-4.f, 1.f, 0.f), 1.0f, new Lambertian(new ConstantTexture(glm::vec3(0.4f, 0.2f, 0.1f))));
	Sphere* pSphere3 = new Sphere(glm::vec3(4.f, 1.f, 0.f), 1.0f, new Metal(new ConstantTexture(glm::vec3(0.7f, 0.6f, 0.5f)), 0.0f));

	m_mapHitables.insert(std::make_pair(pSphere1, HitableType::GEOMETRY));
	m_mapHitables.insert(std::make_pair(pSphere2, HitableType::GEOMETRY));
	m_mapHitables.insert(std::make_pair(pSphere3, HitableType::GEOMETRY));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Scene::CalculateMissColor(glm::vec3 rayDirection)
{
	return m_colMiss;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool Scene::Trace(const Ray& r, int& rayCount, float tmin, float tmax, HitRecord& rec)
{
	++rayCount;

	bool hit_anything = false;
	HitRecord temp_rec;
	float closest_so_far = tmax;

	std::map<Hitable*, HitableType>::iterator iter = m_mapHitables.begin();

	for (; iter != m_mapHitables.end(); ++iter)
	{
		if (iter->first->hit(r, tmin, closest_so_far, temp_rec))
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

