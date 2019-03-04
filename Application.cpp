
#include <thread>
#include <time.h>
#include <vector>
#include <fstream>
#include "glm/glm.hpp"
#include "RayTracer/Hitable.h"
#include "RayTracer/Material.h"
#include "RayTracer/Scene.h"
#include "RayTracer/Helper.h"
#include "RayTracer/Camera.h"
#include "Profiler.h"
#include "Application.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
Application::Application()
{
	m_iBackbufferWidth = 480;
	m_iBackbufferHeight = 270;
	m_iNumSamples = 1;
	m_dTotalRenderTime = 0;
	m_bThreaded = false;

	m_iRayCount = 0;
	m_iRayTriangleQuery = 0;
	m_iRayTriangleSuccess = 0;
	m_iRayBoxQuery = 0;
	m_iRayBoxSuccess = 0;
	m_iTriangleCount = 0;

	m_hWnd = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
	delete m_pCamera;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Initialize(HWND hwnd, bool _threaded)
{
	m_bThreaded = _threaded;
	m_hWnd = hwnd;

	_threaded ? m_iMaxThreads = std::thread::hardware_concurrency() - 2 : 0;

	m_pCamera = &(Camera::getInstance());
	m_pCamera->InitCamera(m_iBackbufferWidth, m_iBackbufferHeight);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Execute()
{
	int percentageDone = 0.0f;

	const clock_t begin_time = clock();
	double counter = 0;

	Trace();

	const clock_t end_time = clock();
	m_dTotalRenderTime = (end_time - begin_time) / (double)CLOCKS_PER_SEC;

	// Write into Profiler...
	Profiler::getInstance().WriteToProfiler("Total Render Time: ", m_dTotalRenderTime);
	Profiler::getInstance().WriteToProfiler("Ray Count: ", m_iRayCount);
	Profiler::getInstance().WriteToProfiler("Ray Triangle Queries : ", m_iRayTriangleQuery);
	Profiler::getInstance().WriteToProfiler("Ray Triangle Success : ", m_iRayTriangleSuccess);
	Profiler::getInstance().WriteToProfiler("Ray Box Queries : ", m_iRayBoxQuery);
	Profiler::getInstance().WriteToProfiler("Ray Box Success : ", m_iRayBoxSuccess);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::SaveImage()
{
	static int count = 0;

	BITMAPINFO info;
	BITMAPFILEHEADER header;
	memset(&info, 0, sizeof(info));
	memset(&header, 0, sizeof(header));

	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = m_iBackbufferWidth;
	info.bmiHeader.biHeight = m_iBackbufferHeight;
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;
	//info.bmiHeader.biSizeImage = width * height * 3;

	header.bfType = 0x4D42;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	char* pixels = NULL;
	HDC hdc = GetDC(m_hWnd);
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP section = CreateDIBSection(hdc, &info, DIB_RGB_COLORS, (void**)&pixels, 0, 0);
	DeleteObject(SelectObject(memDC, section));
	BitBlt(memDC, 0, 0, m_iBackbufferWidth, m_iBackbufferHeight, hdc, 0, 0, SRCCOPY);
	DeleteDC(memDC);

	count++;
	char buf[32] = { 0 };
	sprintf(buf, "RenderImage%d.bmp", count);
	std::fstream hFile(buf, std::ios::out | std::ios::binary);
	if (hFile.is_open())
	{
		hFile.write((char*)&header, sizeof(header));
		hFile.write((char*)&info.bmiHeader, sizeof(info.bmiHeader));
		int bytes = (((24 * m_iBackbufferWidth + 31) & (~31)) / 8) * m_iBackbufferHeight;
		hFile.write(pixels, bytes);
		hFile.close();
	}

	DeleteObject(section);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Application::TraceColor(const Ray & r, int depth, int& rayCount)
{
	HitRecord rec;
	glm::vec3 traceColor = glm::vec3(0);

	if (Scene::getInstance().Trace(r, rayCount, 0.001f, FLT_MAX, rec))
	{
		Ray scatteredRay;
		glm::vec3 attenuation = glm::vec3(0);

		if (depth < 50 && rec.mat_ptr->Scatter(r, rec, rayCount, attenuation, scatteredRay))
		{
			if (glm::distance(scatteredRay.GetRayOrigin(), scatteredRay.GetRayDirection()) < 0.0000001f)
				traceColor = attenuation;
			else
				traceColor = attenuation * TraceColor(scatteredRay, depth + 1, rayCount);
		}
	}
	else
	{
		glm::vec3 unit_direction = glm::normalize(r.GetRayDirection());
		float t = 0.5f * (unit_direction.y + 1.0f);
		traceColor = Helper::LerpVector(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.7f, 1.0f), t);
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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::ParallelTrace(std::mutex * threadMutex, int i)
{
	threadMutex->lock();

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
	HDC hdc = GetDC(m_hWnd);

	int rayCount = 0;

	threadMutex->unlock();

	// Error check for bounds!
	if (startWidth < endWidth && startHeight < endHeight)
	{
		for (int j = startHeight ; j <= endHeight; j++)
		{
			for (int i = startWidth; i <= endWidth; i++)
			{
				glm::vec3 color(0, 0, 0);

				for (int s = 0; s < ns; s++)
				{
					float u = float(i + Helper::GetRandom01()) / float(backBufferWidth);
					float v = float(j + Helper::GetRandom01()) / float(backBufferHeight);

					Ray r = m_pCamera->get_ray(u, v);

					color = color + TraceColor(r, 0, rayCount);
				}

				color = color / float(ns);
				color = glm::vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));

				float ir = (255.99f * color.x);
				float ig = (255.99f * color.y);
				float ib = (255.99f * color.z);

				SetPixel(hdc, backBufferWidth - i, backBufferHeight - j, RGB(ir, ig, ib));
				//++counter;
			}

			//percentageDone = (counter / (gBackbufferWidth*gBackbufferHeight)) * 100.0f;
			//ShowProgress(percentageDone);
		}
	}

	m_iRayCount += rayCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Trace()
{
	if (m_pCamera == nullptr)
		return;

	if (m_bThreaded)
	{
		std::vector<std::thread*> ThreadGroup;
		std::mutex threadMutex;

		for (int i = 0; i < m_iMaxThreads; i++)
		{
			std::thread* t = new std::thread(&Application::ParallelTrace, this, &threadMutex, i);
			ThreadGroup.push_back(t);
		}

		std::vector<std::thread*>::iterator iter = ThreadGroup.begin();
		for (; iter != ThreadGroup.end(); iter++)
		{
			//if((*iter)->joinable())
			(*iter)->join();
		}
	}
	else
	{
		HDC hdc = GetDC(m_hWnd);

		int rayCount = 0;

		for (int j = m_iBackbufferHeight; j >= 0; j--)
		{
			for (int i = 0; i <= m_iBackbufferWidth; i++)
			{
				glm::vec3 color(0, 0, 0);

				for (int s = 0; s < m_iNumSamples; s++)
				{
					float u = float(i + Helper::GetRandom01()) / float(m_iBackbufferWidth);
					float v = float(j + Helper::GetRandom01()) / float(m_iBackbufferHeight);

					Ray r = m_pCamera->get_ray(u, v);

					color = color + TraceColor(r, 0, rayCount);
				}

				color = color / float(m_iNumSamples);
				color = glm::vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));

				float ir = (255.99f*color.x);
				float ig = (255.99f*color.y);
				float ib = (255.99f*color.z);

				//float ir = 255.99f;
				//float ig = 128.99f;
				//float ib = 255.99f;

				//fprintf(filePtr, "\n%d %d %d", ir, ig, ib);
				SetPixel(hdc, m_iBackbufferWidth - i, m_iBackbufferHeight - j, RGB(ir, ig, ib));
				//++counter;
			}

			//percentageDone = (counter / (gBackbufferWidth*gBackbufferHeight)) * 100.0f;
			//ShowProgress(percentageDone);
		}

		m_iRayCount += rayCount;
	}
}