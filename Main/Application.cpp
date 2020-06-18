
#include <thread>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "glm/glm.hpp"
#include "glm\gtc\type_ptr.hpp"

#include "../RayTracer/Hitable.h"
#include "../RayTracer/Material.h"
#include "../RayTracer/Scene.h"
#include "../RayTracer/Helper.h"
#include "../RayTracer/Camera.h"
#include "..//RayTracer/Scene.h"
#include "../RayTracer/Sampler.h"
#include "Profiler.h"
#include "ScreenAlignedQuad.h"
#include "DebugLines.h"
#include "Application.h"

#include "marl/defer.h"
#include "marl/scheduler.h"
#include "marl/thread.h"
#include "marl/waitgroup.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define CPLUSPLUS_THREADING 
#define ACCUM_BUFFER_RENDERING 
#define SAVE_IMAGE
#define DENOISE_IMAGE
//#define MARL_SCHEDULING 1

///////////////////////////////////////////////////////////////////////////////////////////////////
Application::Application()
{
	m_iBackbufferWidth = 960;
	m_iBackbufferHeight = 540;

#if defined _DEBUG
	m_iNumSamples = 1;
#else 
	m_iNumSamples = 512;
#endif

	m_dTotalRenderTime = 0;
	m_dDenoiserTime = 0;
	m_bThreaded = false;

	m_iRayCount = 0;
	m_iRayTriangleQuery = 0;
	m_iRayTriangleSuccess = 0;
	m_iRayBoxQuery = 0;
	m_iRayBoxSuccess = 0;
	m_iTriangleCount = 0;

	m_pQuad = nullptr;
	m_pSampler = nullptr;
	m_pDebugLines = nullptr;

	m_bSinglePixelMode = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
	if (m_pSampler)
	{
		delete m_pSampler;
		m_pSampler = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Initialize(bool _threaded)
{
	m_bThreaded = _threaded;

	_threaded ? m_iMaxThreads = std::thread::hardware_concurrency() : 0;

	Scene::getInstance().InitRefractionScene(m_iBackbufferWidth, m_iBackbufferHeight);
	//Scene::getInstance().InitSphereScene(m_iBackbufferWidth, m_iBackbufferHeight);
	//Scene::getInstance().InitCornellScene(m_iBackbufferWidth, m_iBackbufferHeight);
	//Scene::getInstance().InitMaterialTestScene(m_iBackbufferWidth, m_iBackbufferHeight);
	//Scene::getInstance().InitTigerScene(m_iBackbufferWidth, m_iBackbufferHeight);
	//Scene::getInstance().InitTowerScene(m_iBackbufferWidth, m_iBackbufferHeight);

	m_pQuad = new ScreenAlignedQuad();
	m_pQuad->Init(m_iBackbufferWidth, m_iBackbufferHeight);

	// Initialize Sampler
	m_pSampler = new JitteredSampler();
	m_pSampler->GenerateSamples(m_iNumSamples);

	glm::vec3 col = glm::vec3(0, 0, 0);
	for (int i = 0; i < m_iBackbufferWidth * m_iBackbufferHeight; i++)
	{
		m_vecSrcPixels.push_back(col);
		m_vecDstPixels.push_back(col);
	}

	m_pDebugLines = new DebugLines();

	// Create Open Image Denoise Device
	m_oidnDevice = oidn::newDevice();
	m_oidnDevice.set("numThreads", m_iMaxThreads);
	m_oidnDevice.commit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Execute(GLFWwindow* window)
{
	int percentageDone = 0.0f;

	const clock_t begin_time = clock();
	double counter = 0;

	Trace(window);

	const clock_t end_time = clock();
	m_dTotalRenderTime = (end_time - begin_time) / (double)CLOCKS_PER_SEC;

	// Write into Profiler...
	Profiler::getInstance().WriteToProfiler("Total Render Time: ", (float)m_dTotalRenderTime);
	Profiler::getInstance().WriteToProfiler("Ray Count: ", m_iRayCount);
	Profiler::getInstance().WriteToProfiler("Ray Triangle Queries : ", m_iRayTriangleQuery);
	Profiler::getInstance().WriteToProfiler("Ray Triangle Success : ", m_iRayTriangleSuccess);
	Profiler::getInstance().WriteToProfiler("Ray Box Queries : ", m_iRayBoxQuery);
	Profiler::getInstance().WriteToProfiler("Ray Box Success : ", m_iRayBoxSuccess);

	std::cout << "------------------- Profile Information -------------------" << Profiler::getInstance().GetProfilerTexts().c_str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::SaveImage()
{
#if defined SAVE_IMAGE
	std::string fileName = m_pSampler->GetName() + "_"
												 + std::to_string(m_iNumSamples)
												 + "SPP"
												 + "_"
												 + Helper::GetCurrentDateTime()
												 + ".hdr";

	stbi_flip_vertically_on_write(1);
	stbi_write_hdr(fileName.c_str(), m_iBackbufferWidth, m_iBackbufferHeight, 3, glm::value_ptr(m_vecDstPixels[0]));
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::DenoiseImage()
{
#if defined DENOISE_IMAGE
	float* pixelData = (float*)malloc(3 * m_iBackbufferWidth * m_iBackbufferHeight * sizeof(float));
	float* outData   = (float*)malloc(3 * m_iBackbufferWidth * m_iBackbufferHeight * sizeof(float));
	if (pixelData && outData)
	{
		float* origData = glm::value_ptr(m_vecDstPixels[0]);
		
#if defined _DEBUG
		// Write down orginal image in HDR format
		stbi_write_hdr("debug.hdr", m_iBackbufferWidth, m_iBackbufferHeight, 3, origData);
#endif

		// Create a denoising filter
		m_oidnFilter = m_oidnDevice.newFilter("RT");
		m_oidnFilter.setImage("color", origData, oidn::Format::Float3, m_iBackbufferWidth, m_iBackbufferHeight);
		m_oidnFilter.setImage("output", outData, oidn::Format::Float3, m_iBackbufferWidth, m_iBackbufferHeight);
		m_oidnFilter.set("hdr", true);
		m_oidnFilter.set("numThreads", m_iMaxThreads);
		m_oidnFilter.set("setAffinity", true);
		m_oidnFilter.commit();

		const clock_t begin_time = clock();
		
		m_oidnFilter.execute();

		const clock_t end_time = clock();
		m_dDenoiserTime = (end_time - begin_time) / (float)CLOCKS_PER_SEC;
		Profiler::getInstance().WriteToProfiler("Denoiser Time: ", m_dDenoiserTime);

#if defined _DEBUG
		// Check for errors
		const char* errorMessage;
		if (m_oidnDevice.getError(errorMessage) != oidn::Error::None)
		{
			Profiler::getInstance().WriteToProfiler(errorMessage);
		}
#endif

		// Write down denoised image in HDR format!!
		std::string fileName = m_pSampler->GetName() + "_"
													 + std::to_string(m_iNumSamples)		
													 + "SPP"
													 + "_"		
													 + Helper::GetCurrentDateTime()
													 + "_denoised.hdr";

		stbi_write_hdr(fileName.c_str(), m_iBackbufferWidth, m_iBackbufferHeight, 3, outData);
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Application::TraceColor(const Ray & r, int depth, int& rayCount)
{
	HitRecord rec;
	glm::vec3 traceColor = glm::vec3(0.0f, 0.0f, 0.0f);

	// if we have exceeded ray bounce limit, no more light is gathered!
	if (depth >= 5)
		return traceColor;

	if (Scene::getInstance().Trace(r, rayCount, 0.001f, FLT_MAX, rec))
	{
		Ray scatteredRay;
		float materialPDF;

		glm::vec3 outColor = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 emitted = rec.mat_ptr->Emitted(r, rec);

		// If we hit Emissive material, return Emissive color as it is!
		if (!rec.mat_ptr->Scatter(r, rec, rayCount, outColor, scatteredRay, materialPDF))
			return emitted;

		if (m_bSinglePixelMode)
		{
			if(depth == 1)
				m_pDebugLines->AddToLines(r.origin, rec.P, glm::vec4(1, 0.85f, 0, 1));
			else if(depth == 2)
				m_pDebugLines->AddToLines(r.origin, rec.P, glm::vec4(0.5f, 1, 0, 1));
			else if(depth == 3)
				m_pDebugLines->AddToLines(r.origin, rec.P, glm::vec4(1, 0.27f, 0, 1));
			else if(depth == 4)			  
				m_pDebugLines->AddToLines(r.origin, rec.P, glm::vec4(0.11f, 0.56f, 1.0f, 1));
			else if(depth > 4)
				m_pDebugLines->AddToLines(r.origin, rec.P, glm::vec4(0.73f, 0.33f, 0.83f, 1));
		}
			

		//! Light PDF
		//std::map<Hitable*, HitableType>::iterator iter = Scene::getInstance().m_mapHitables.begin();
		//std::map<Hitable*, HitableType>::iterator iterEnd = Scene::getInstance().m_mapHitables.end();
		//
		//float lightPDF = 0.0f;
		//Ray lightSampleRay;
		//
		//for (; iter != iterEnd; ++iter)
		//{
		//	if (iter->second == HitableType::LIGHT)
		//	{
		//		glm::vec3 lightSampleVector = iter->first->Sample(rec.P);
		//
		//		////glm::vec3 tar = rec.P + lightSampleVector;
		//		lightSampleRay = Ray(rec.P, lightSampleVector);
		//		lightPDF = iter->first->PDF(rec.P, lightSampleRay.direction);
		//	}
		//}
		//
		//Ray finalRay;
		//if (Helper::GetRandom01() < 0.5f)
		//	finalRay = scatteredRay;
		//else
		//	finalRay = lightSampleRay;
		//
		//float finalPDF = 0.5f * materialPDF + 0.5f * lightPDF;

		//if(rec.mat_ptr->m_eType == MaterialType::METAL && (Helper::GetRandom01() < 0.8f && Helper::GetRandom01() > 0.9f))
		//	m_pDebugLines->AddToLines(scatteredRay.origin, scatteredRay.origin + scatteredRay.direction, glm::vec4(0.2f, 0.2f, 0.2f, 1));

		traceColor = outColor * TraceColor(scatteredRay, depth + 1, rayCount) / materialPDF;
	}
	else
	{
		glm::vec3 unit_direction = glm::normalize(r.direction);
		//float t = 0.5f * (unit_direction[1] + 1.0f);
		//traceColor = Helper::LerpVector(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.7f, 1.0f), t);
		traceColor = Scene::getInstance().CalculateMissColor(unit_direction);
	}

	// debug info...
	m_iRayTriangleQuery += rec.rayTriangleQuery;
	m_iRayTriangleSuccess += rec.rayTriangleSuccess;
	m_iRayBoxQuery += rec.rayBoxQuery;
	m_iRayBoxSuccess += rec.rayBoxSuccess;

	return traceColor;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::ShowProgress(int percentage)
{
	system("cls");
	printf("\nRendering Progress : %d%%\n", percentage);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::ParallelTrace(std::mutex * threadMutex, int i, GLFWwindow* window)
{
	//threadMutex->lock();

	int backBufferHeight = m_iBackbufferHeight;
	int backBufferWidth = m_iBackbufferWidth;
	int quarterHeight = m_iBackbufferHeight / m_iMaxThreads;
	
	int startWidth = 0;
	int endWidth = m_iBackbufferWidth;
	
  	
	int startHeight, endHeight;
	
	if (i == 0)
		startHeight = (i * quarterHeight);
	else
		startHeight = (i * quarterHeight) + i;

	if (i < m_iMaxThreads - 1)
		endHeight = startHeight + quarterHeight;
	else
		endHeight = backBufferHeight;

	int ns = m_iNumSamples;
	GLFWwindow* hWindow = window;

	int rayCount = 0;

	//threadMutex->unlock();

	std::vector<glm::vec2> samples = m_pSampler->GetSamples();


#if defined ACCUM_BUFFER_RENDERING
	// Error check for bounds!
	for (int s = 0; s < ns; ++s)
	{
		if (startWidth < endWidth && startHeight < endHeight)
		{
			for (int j = startHeight; j <= endHeight; j++)
			{
				for (int i = startWidth; i <= endWidth; i++)
				{
					glm::vec3 color(0, 0, 0);

					float u = float(i + samples[s].x);
					float v = float(j + samples[s].y);

					Ray r = Camera::getInstance().get_ray(u, v);

					color = color + TraceColor(r, 0, rayCount);

					//color = color / float(ns);
					color = glm::vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));

					//threadMutex->lock();
					//!-- Accumulative buffer
					// So instead of running all the samples in each iteration, we run one sample
					// in each iteration & acumulate the result in final buffer!
					m_vecSrcPixels[j * endWidth + i] += (color / (float)ns);
					//threadMutex->unlock();
				}
			}
		}
	}
#else
	// Error check for bounds!
	if (startWidth < endWidth && startHeight < endHeight)
	{
		for (int j = startHeight; j <= endHeight; j++)
		{
			for (int i = startWidth; i <= endWidth; i++)
			{
				glm::vec3 color(0, 0, 0);

				for (int s = 0; s < ns; s++)
				{
					float u = float(i + samples[s].x);
					float v = float(j + samples[s].y);

					Ray r = Camera::getInstance().get_ray(u, v);

					color = color + TraceColor(r, 0, rayCount);
				}

				color = color / float(ns);
				color = glm::vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));

				//threadMutex->lock();
				m_vecSrcPixels[j * endWidth + i] = color;
				//threadMutex->unlock();
			}
		}
	}
#endif // ACCUM_BUFFER_RENDERING

	
	

	m_iRayCount += rayCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::RenderPixel(int rowIndex, int columnIndex)
{
	m_bSinglePixelMode = true;

	glm::vec3 color(0, 0, 0);
	int rayCount = 0;

	std::vector<glm::vec2> samples = m_pSampler->GetSamples();

	for (int s = 0; s < 1; s++)
	{
		float u = float(rowIndex); //+samples[s].x);
		float v = float(m_iBackbufferHeight - columnIndex);// +samples[s].y);

		Ray r = Camera::getInstance().get_ray(u,v);

		color = color + TraceColor(r, 0, rayCount);
	}

	//color = color / float(m_iNumSamples);
	//color = glm::vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));
	//
	//int index = columnIndex * m_iBackbufferHeight + rowIndex;
	//if (index < m_iBackbufferWidth * m_iBackbufferHeight)
	//{
	//	m_vecSrcPixels[index] = glm::vec3(1,0,0);
	//}

	m_pDebugLines->InitializeLines();
	m_bSinglePixelMode = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::UpdateGL(GLFWwindow* window)
{
	// Message Loop!
	while (!glfwWindowShouldClose(window))
	{
		m_vecDstPixels = m_vecSrcPixels;
	
		glfwPollEvents();
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		m_pQuad->UpdateTexture(0, 0, m_iBackbufferWidth, m_iBackbufferHeight, glm::value_ptr(m_vecDstPixels[0]));
		m_pQuad->Render();

		m_pDebugLines->RenderLines();
	
		glfwSwapBuffers(window);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Trace(GLFWwindow* window)
{
#if defined _DEBUG

	int rayCount = 0;
	std::vector<glm::vec2> samples = m_pSampler->GetSamples();

	for (int j = 0; j < m_iBackbufferHeight; j++)
	{
		std::vector<glm::vec3> rowColor;
		for (int i = 0; i < m_iBackbufferWidth; i++)
		{
			glm::vec3 color(0, 0, 0);

			for (int s = 0; s < m_iNumSamples; s++)
			{
				float u = float(i + samples[s].x);
				float v = float(j + samples[s].y);

				Ray r = Camera::getInstance().get_ray(u, v);

				color = color + TraceColor(r, 0, rayCount);
			}
			
			color = color / float(m_iNumSamples);
			color = glm::vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));

			m_vecSrcPixels[j * m_iBackbufferWidth + i] = color;
			rowColor.push_back(color);
		}

		m_pQuad->UpdateTexture(0, j, m_iBackbufferWidth, 1, glm::value_ptr(rowColor[0]));
		m_pQuad->Render();
		glfwSwapBuffers(window);
		rowColor.clear();
	}
	
	m_pDebugLines->InitializeLines();

	m_iRayCount += rayCount;

#else

#if defined CPLUSPLUS_THREADING 
	std::vector<std::thread*> ThreadGroup;
	std::mutex threadMutex;

	for (int i = 0; i < m_iMaxThreads; i++)
	{
		std::thread* t = new std::thread(&Application::ParallelTrace, this, &threadMutex, i, window);
		ThreadGroup.push_back(t);
	}

	std::vector<std::thread*>::iterator iter = ThreadGroup.begin();
	for (; iter != ThreadGroup.end(); iter++)
	{
		(*iter)->detach();
	}
#elif defined MARL_SCHEDULING
	// Marl Scheduler related...
	marl::Scheduler scheduler;
	scheduler.setWorkerThreadCount(marl::Thread::numLogicalCPUs());
	scheduler.bind();
	defer(scheduler.unbind());

	marl::WaitGroup wg(m_iBackbufferHeight);

	for (uint32_t y = 0; y < m_iBackbufferHeight; y++)
	{
		marl::schedule([=] {

			defer(wg.done());

			for (uint32_t x = 0; x < m_iBackbufferWidth; x++)
			{
				RenderPixel(y, x);
			}
			});
	}

	wg.wait();
#endif

#endif

	
}