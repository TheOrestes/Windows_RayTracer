// WindowsRayTracer.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include <vector>
#include <string>
#include <thread>
#include <mutex>

/////------- Ray Tracer based Includes -------/////
#include "glm/glm.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "../RayTracer/Ray.h"
#include "../RayTracer/Sphere.h"
#include "../RayTracer/Triangle.h"
#include "../RayTracer/Scene.h"
#include "../RayTracer/Camera.h"
#include "../RayTracer/Helper.h"
#include "../RayTracer/Material.h"
#include "../RayTracer/Lambertian.h"
#include "../RayTracer/Metal.h"
#include "../RayTracer/Transparent.h"
#include "ScreenAlignedQuad.h"

const int COLOR_CHANNELS = 3; // RGB
const int gBackbufferWidth = 480;
const int gBackbufferHeight = 270;
const int nSamples = 1;

unsigned long long int numRays = 0;

int maxNumThreads = 0;
double TotalRenderTime = 0;

GLFWwindow* window = nullptr;
Camera* gCam = nullptr;
ScreenAlignedQuad* gQuad = nullptr;

glm::vec3 gColorBuffer[gBackbufferWidth][gBackbufferHeight];
float** ppColorBuffer = nullptr;

std::vector<glm::vec3> vecBuffer;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region RayTracer

//Hitable* BasicTestScene()
//{
//	Hitable** list = new Hitable*[6];
//	list[0] = new Sphere(glm::vec3(1.05f, 0, 0), 0.5, new Metal(glm::vec3(0.5, 0.2, 0.1), 0.5));
//	list[1] = new Sphere(glm::vec3(0, -100.5, 0), 100, new Lambertian(glm::vec3(0.2, 0.2, 0.2)));
//	list[2] = new Sphere(glm::vec3(0, 0, 2), 0.5, new Lambertian(glm::vec3(1.0f, 0.0f, 0.0f)));
//	list[3] = new Sphere(glm::vec3(-1.05f, 0, 0), 0.5, new Metal(glm::vec3(1.0, 0.2, 0.0), 0));
//	list[4] = new Sphere(glm::vec3(0.0f, 0, -3), 0.5, new Lambertian(glm::vec3(1.0, 1.0, 0.0)));
//	list[5] = new Triangle(glm::vec3(-2.0f, 0.0f, -1.0f), glm::vec3(2.0f, 0.0f, -1.0f), glm::vec3(0.0f, 2.0f, -1.0f), new Lambertian(glm::vec3(0.0f, 1.0f, 0.0f)));
//
//	return new HitableList(list, 6);
//}
//
//Hitable* random_scene()
//{
//	int n = 500;
//	Hitable** list = new Hitable*[n + 1];
//	list[0] = new Sphere(glm::vec3(0, -1000, 0), 1000, new Lambertian(glm::vec3(0.5, 0.5, 0.5)));
//	int i = 1;
//	for (int a = -11; a < 11; a++)
//	{
//		for (int b = -11; b < 11; b++)
//		{
//			float choose_mat = Helper::GetRandom01();
//			glm::vec3 center(a + 0.9f*Helper::GetRandom01(), 0.2, b + 0.9*Helper::GetRandom01());
//			if ((center - glm::vec3(4, 0.2, 0)).Length() > 0.9f)
//			{
//				if (choose_mat < 0.8f)
//				{
//					// diffuse
//					list[i++] = new Sphere(center, 0.2f, new Lambertian(glm::vec3(Helper::GetRandom01()*Helper::GetRandom01(), Helper::GetRandom01()*Helper::GetRandom01(), Helper::GetRandom01()*Helper::GetRandom01())));
//				}
//				else if (choose_mat < 0.95)
//				{
//					// Metal
//					list[i++] = new Sphere(center, 0.2f, new Metal(glm::vec3(0.5f*(1 + Helper::GetRandom01()), 0.5f*(1 + Helper::GetRandom01()), 0.5f*(1 + Helper::GetRandom01())), Helper::GetRandom01()));
//				}
//				else
//				{
//					// glass
//					list[i++] = new Sphere(center, 0.2f, new Transparent(1.5f));
//				}
//			}
//		}
//	}
//
//	list[i++] = new Sphere(glm::vec3(0, 1, 0), 1.0f, new Transparent(1.5f));
//	list[i++] = new Sphere(glm::vec3(-4, 1, 0), 1.0f, new Lambertian(glm::vec3(0.4f, 0.2f, 0.1f)));
//	list[i++] = new Sphere(glm::vec3(4, 1, 0), 1.0f, new Metal(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f));
//
//	return new HitableList(list, i);
//}
//
//Hitable* world = BasicTestScene();
glm::vec3 TraceColor(const Ray& r, int depth)
{
	HitRecord rec;

	++numRays;
	if (Scene::getInstance().Trace(r, 0.001f, FLT_MAX, rec))
	{
		Ray scatteredRay;
		glm::vec3 attenuation;

		if (depth < 50 && rec.mat_ptr->Scatter(r, rec, attenuation, scatteredRay))
		{
			return attenuation * TraceColor(scatteredRay, depth + 1);
		}
		else
		{
			return glm::vec3(0, 0, 0);
		}
	}
	else
	{
		glm::vec3 unit_direction = glm::normalize(r.GetRayDirection());
		float t = 0.5 * (unit_direction.y + 1.0f);
		return Helper::LerpVector(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.7f, 1.0f), t);
	}
}

void ShowProgress(int percentage)
{
	system("cls");
	printf("\nRendering Progress : %d%%\n", percentage);
}

void ParallelTrace(std::mutex* threadMutex, int i)
{
	threadMutex->lock();

	int backBufferHeight = gBackbufferHeight;
	int backBufferWidth = gBackbufferWidth;
	int quarterHeight = gBackbufferHeight / maxNumThreads;
	int startWidth = 0;
	int startHeight = i * quarterHeight;
	int endWidth = gBackbufferWidth;
	int endHeight = (i + 1) * quarterHeight;
	int ns = nSamples;
	GLFWwindow* hWindow = window;

	threadMutex->unlock();

	// Error check for bounds!
	if (startWidth < endWidth && startHeight < endHeight)
	{
		for (int j = startHeight; j <= endHeight; j++)
		{
			std::vector<glm::vec3> rowColor;

			for (int i = startWidth; i <= endWidth; i++)
			{
				glm::vec3 color(0, 0, 0);

				for (int s = 0; s < ns; s++)
				{
					float u = float(i + Helper::GetRandom01()) / float(backBufferWidth);
					float v = float(j + Helper::GetRandom01()) / float(backBufferHeight);

					Ray r = gCam->get_ray(u, v);

					color = color + TraceColor(r, 0);
				}

				color = color / float(ns);
				color = glm::vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));

				threadMutex->lock();
				
				vecBuffer[j * endHeight + i] = color;

				threadMutex->unlock();

				//rowColor.push_back(color);
				
				//SetPixel(hdc, backBufferWidth - i, backBufferHeight - j, RGB(ir, ig, ib));
				//++counter;
			}

			//gQuad->UpdateTexture(i, j, backBufferWidth, 1, glm::value_ptr(rowColor[0]));
			//gQuad->Render();
			//glfwSwapBuffers(hWindow);
			rowColor.clear();
		}
	}
}


void UpdateGL()
{
	gQuad->UpdateTexture(0, 0, gBackbufferWidth, gBackbufferHeight, glm::value_ptr(vecBuffer[0]));
	gQuad->Render();
	glfwSwapBuffers(window);
}

void Trace()
{
	if (gCam == nullptr)
		return;

#pragma region OLD_CODE

	for (int j = gBackbufferHeight; j >= 0; j--)
	{
		std::vector<glm::vec3> rowColors;
	
		for (int i = 0; i <= gBackbufferWidth; i++)
		{
			glm::vec3 color(0, 0, 0);
	
			for (int s = 0; s < nSamples; s++)
			{
				float u = float(i + Helper::GetRandom01()) / float(gBackbufferWidth);
				float v = float(j + Helper::GetRandom01()) / float(gBackbufferHeight);
	
				Ray r = gCam->get_ray(u, v);
	
				color = color + TraceColor(r, 0);
			}
	
			color = color / float(nSamples);
			color = glm::vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));
	
			//SetPixel(hdc, gBackbufferWidth - i, gBackbufferHeight - j, RGB(ir, ig, ib));
	
			rowColors.push_back(color);
		}
	
		gQuad->UpdateTexture(0, j, gBackbufferWidth, 1, glm::value_ptr(rowColors[0]));
		gQuad->Render();
		glfwSwapBuffers(window);
		rowColors.clear();
		//percentageDone = (counter / (gBackbufferWidth*gBackbufferHeight)) * 100.0f;
		//ShowProgress(percentageDone);
	}
#pragma endregion

	//std::vector<std::thread*> ThreadGroup;
	//std::mutex threadMutex;
	//
	//for (int i = 0; i < maxNumThreads; i++)
	//{
	//	std::thread* t = new std::thread(&ParallelTrace, &threadMutex, i);
	//	ThreadGroup.push_back(t);
	//}
	//
	//std::vector<std::thread*>::iterator iter = ThreadGroup.begin();
	//for (; iter != ThreadGroup.end(); iter++)
	//{
	//	//if((*iter)->joinable())
	//	(*iter)->join();
	//}
}

void Execute()
{

	int percentageDone = 0.0f;

	const clock_t begin_time = clock();
	double counter = 0;

	Trace();

	const clock_t end_time = clock();
	TotalRenderTime = (end_time - begin_time) / (double)CLOCKS_PER_SEC;

	const size_t len = 256;
	wchar_t buffer[len] = {};
	swprintf(buffer, L"Windows Ray Tracer [Render Time : %0.2f seconds!]", TotalRenderTime);
	//SetWindowText(hWnd, buffer);
	//MessageBox(hWnd, buffer, L"Render Time!", MB_OKCANCEL);

	//printf("Render Time : %.2f seconds\n", time);

}

#pragma endregion
////////////////////////////////////////// END OF RAY TRACER CODE ///////////////////////////////////////////

void InitGLFW()
{
	// Initialize & Setup basic 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a window!
	window = glfwCreateWindow(gBackbufferWidth, gBackbufferHeight, "Ray Tracer", nullptr, nullptr);

	if (!window)
	{
		std::cout << "Create Window FAILED!!!\n";
		glfwTerminate();
		return;
	}

	// Window is created, now create context for the same window...
	glfwMakeContextCurrent(window);
}

void InitGLEW()
{
	// Ensure glew uses all the modern techniques...
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Initialize GLEW FAILED!!!\n";
		return;
	}
}

//////////////////////////////////////////////////////////////////////////
// 3. Inputs
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		
	}
}

int main()
{
	InitGLFW();
	InitGLEW();

	glfwSetKeyCallback(window, KeyHandler);

	// get maximum number of supported threads!
	maxNumThreads = std::thread::hardware_concurrency();

	// Initialize Screen Aligned Quad
	gQuad = new ScreenAlignedQuad();
	gQuad->Init(gBackbufferWidth, gBackbufferHeight);

	// Initialize global color buffer
	//vecBuffer.resize(gBackbufferWidth*gBackbufferHeight);
	glm::vec3 col = glm::vec3(1,0,0);
	for (int i = 0; i < gBackbufferWidth * gBackbufferHeight; i++)
	{
		vecBuffer.push_back(col);
	}
	
	// Initialize Camera!
	Camera::getInstance().InitCamera(gBackbufferWidth, gBackbufferHeight);
	gCam = &Camera::getInstance();

	Execute();

	glfwTerminate();

	return 0;
}


void SaveImage()
{
	//static int count = 0;
	//
	//BITMAPINFO info;
	//BITMAPFILEHEADER header;
	//memset(&info, 0, sizeof(info));
	//memset(&header, 0, sizeof(header));
	//
	//info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	//info.bmiHeader.biWidth = gBackbufferWidth;
	//info.bmiHeader.biHeight = gBackbufferHeight;
	//info.bmiHeader.biPlanes = 1;
	//info.bmiHeader.biBitCount = 24;
	//info.bmiHeader.biCompression = BI_RGB;
	////info.bmiHeader.biSizeImage = width * height * 3;
	//
	//header.bfType = 0x4D42;
	//header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	//
	//char* pixels = NULL;
	//HDC hdc = GetDC(hWnd);
	//HDC memDC = CreateCompatibleDC(hdc);
	//HBITMAP section = CreateDIBSection(hdc, &info, DIB_RGB_COLORS, (void**)&pixels, 0, 0);
	//DeleteObject(SelectObject(memDC, section));
	//BitBlt(memDC, 0, 0, gBackbufferWidth, gBackbufferHeight, hdc, 0, 0, SRCCOPY);
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
	//	int bytes = (((24 * gBackbufferWidth + 31) & (~31)) / 8) * gBackbufferHeight;
	//	hFile.write(pixels, bytes);
	//	hFile.close();
	//}
	//
	//DeleteObject(section);
}


