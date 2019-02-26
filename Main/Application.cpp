
#include <thread>
#include <time.h>
#include <vector>
#include <fstream>
#include "glm/glm.hpp"
#include "glm\gtc\type_ptr.hpp"

#include "../RayTracer/Hitable.h"
#include "../RayTracer/Material.h"
#include "../RayTracer/Scene.h"
#include "../RayTracer/Helper.h"
#include "../RayTracer/Camera.h"
#include "ScreenAlignedQuad.h"
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

	m_pCamera = nullptr;
	m_pQuad = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Application::~Application()
{
	delete m_pCamera;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Initialize(bool _threaded)
{
	m_bThreaded = _threaded;

	_threaded ? m_iMaxThreads = std::thread::hardware_concurrency() : 0;

	m_pCamera = &(Camera::getInstance());
	m_pCamera->InitCamera(m_iBackbufferWidth, m_iBackbufferHeight);

	m_pQuad = new ScreenAlignedQuad();
	m_pQuad->Init(m_iBackbufferWidth, m_iBackbufferHeight);

	glm::vec3 col = glm::vec3(1, 0, 0);
	for (int i = 0; i < m_iBackbufferWidth * m_iBackbufferHeight; i++)
	{
		vecBuffer.push_back(col);
	}
	std::copy(vecBuffer.begin(), vecBuffer.end(), std::back_inserter(copyBuffer));
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

	int rayCount = m_iRayCount;

	const size_t len = 256;
	char buffer[len] = {};
	sprintf(buffer, "[Time : %0.2f seconds!] [Ray Count : %d rays]", m_dTotalRenderTime, rayCount);
	glfwSetWindowTitle(window, buffer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::SaveImage()
{
	static int count = 0;

	//BITMAPINFO info;
	//BITMAPFILEHEADER header;
	//memset(&info, 0, sizeof(info));
	//memset(&header, 0, sizeof(header));
	//
	//info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//info.bmiHeader.biWidth = m_iBackbufferWidth;
	//info.bmiHeader.biHeight = m_iBackbufferHeight;
	//info.bmiHeader.biPlanes = 1;
	//info.bmiHeader.biBitCount = 24;
	//info.bmiHeader.biCompression = BI_RGB;
	////info.bmiHeader.biSizeImage = width * height * 3;
	//
	//header.bfType = 0x4D42;
	//header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	//
	//char* pixels = NULL;
	//HDC hdc = GetDC(m_hWnd);
	//HDC memDC = CreateCompatibleDC(hdc);
	//HBITMAP section = CreateDIBSection(hdc, &info, DIB_RGB_COLORS, (void**)&pixels, 0, 0);
	//DeleteObject(SelectObject(memDC, section));
	//BitBlt(memDC, 0, 0, m_iBackbufferWidth, m_iBackbufferHeight, hdc, 0, 0, SRCCOPY);
	//DeleteDC(memDC);
	//
	//count++;
	//char buf[32] = { 0 };
	//sprintf(buf, "RenderImage%d.bmp", count);
	//std::fstream hFile(buf, std::ios::out | std::ios::binary);
	//if (hFile.is_open())
	//{
	//	hFile.write((char*)&header, sizeof(header));
	//	hFile.write((char*)&info.bmiHeader, sizeof(info.bmiHeader));
	//	int bytes = (((24 * m_iBackbufferWidth + 31) & (~31)) / 8) * m_iBackbufferHeight;
	//	hFile.write(pixels, bytes);
	//	hFile.close();
	//}
	//
	//DeleteObject(section);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 Application::TraceColor(const Ray & r, int depth, int& rayCount)
{
	HitRecord rec;

	if (Scene::getInstance().Trace(r, rayCount, 0.001f, FLT_MAX, rec))
	{
		Ray scatteredRay;
		glm::vec3 attenuation = glm::vec3(0);

		if (depth < 50 && rec.mat_ptr->Scatter(r, rec, rayCount, attenuation, scatteredRay))
		{
			if (glm::distance(scatteredRay.GetRayOrigin(), scatteredRay.GetRayDirection()) < 0.0000001f)
				return attenuation;
			else
				return attenuation * TraceColor(scatteredRay, depth + 1, rayCount);
		}
		else
		{
			return glm::vec3(0, 0, 0);
		}
	}
	else
	{
		glm::vec3 unit_direction = glm::normalize(r.GetRayDirection());
		float t = 0.5f * (unit_direction.y + 1.0f);
		return Helper::LerpVector(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.7f, 1.0f), t);
	}
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
	threadMutex->lock();

	int backBufferHeight = m_iBackbufferHeight;
	int backBufferWidth = m_iBackbufferWidth;
	int quarterHeight = m_iBackbufferHeight / m_iMaxThreads;
	int startWidth = 0;
	int startHeight = i * quarterHeight;
	int endWidth = m_iBackbufferWidth;
	int endHeight = (i + 1) * quarterHeight;
	int ns = m_iNumSamples;
	GLFWwindow* hWindow = window;

	int rayCount = 0;

	threadMutex->unlock();

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
					float u = float(i + Helper::GetRandom01()) / float(backBufferWidth);
					float v = float(j + Helper::GetRandom01()) / float(backBufferHeight);

					Ray r = m_pCamera->get_ray(u, v);

					color = color + TraceColor(r, 0, rayCount);
				}

				color = color / float(ns);
				color = glm::vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));

				threadMutex->lock();
				vecBuffer[j * endWidth + i] = color;
				threadMutex->unlock();
			}
		}
	}

	m_iRayCount += rayCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::UpdateGL(GLFWwindow* window)
{
	copyBuffer.clear();
	std::copy(vecBuffer.begin(), vecBuffer.end(), std::back_inserter(copyBuffer));

	m_pQuad->UpdateTexture(0, 0, m_iBackbufferWidth, m_iBackbufferHeight, glm::value_ptr(vecBuffer[0]));
	m_pQuad->Render();

	glfwSwapBuffers(window);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Application::Trace(GLFWwindow* window)
{
	if (m_pCamera == nullptr)
		return;

	if (m_bThreaded)
	{
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
			//if((*iter)->joinable())
			(*iter)->join();
		}
	}
	else
	{
		int rayCount = 0;

		for (int j = 0; j < m_iBackbufferHeight; j++)
		{
			std::vector<glm::vec3> rowColor;
			for (int i = 0; i < m_iBackbufferWidth; i++)
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

				//vecBuffer[j * gBackbufferWidth + i] = color;

				rowColor.push_back(color);
			}

			m_pQuad->UpdateTexture(0, j, m_iBackbufferWidth, 1, glm::value_ptr(rowColor[0]));
			m_pQuad->Render();
			glfwSwapBuffers(window);
			rowColor.clear();
		}

		m_iRayCount += rayCount;
	}
}