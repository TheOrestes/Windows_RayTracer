// WindowsRayTracer.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include <vector>
#include <string>
#include <thread>
#include <mutex>

#include "Application.h"
#include "Profiler.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
GLFWwindow* InitGLFW(Application* pApp)
{
	if (!pApp)
	{
		std::cout << "Application Pointer not initialized!\n";
		return nullptr;
	}

	// Initialize & Setup basic  
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a window!
	GLFWwindow* window = glfwCreateWindow(pApp->GetBufferWidth(), pApp->GetBufferHeight(), "Ray Tracer", nullptr, nullptr);

	if (!window)
	{
		std::cout << "Create Window FAILED!!!\n";
		glfwTerminate();
		return nullptr;
	}

	// Window is created, now create context for the same window...
	glfwMakeContextCurrent(window);

	return window;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////////
// 3. Inputs
void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	GLFWwindow* window;
	Application* pApp = new Application();

	window = InitGLFW(pApp);
	if (!window)
		return 0;

	InitGLEW();

	glfwSetKeyCallback(window, KeyHandler);		

#ifdef NDEBUG
	pApp->Initialize(true);
#else
	pApp->Initialize(false);
#endif

	pApp->Execute(window);
	pApp->UpdateGL(window);

	pApp->SaveImage();
	pApp->DenoiseImage();

	glfwTerminate();

	delete pApp;

	return 0;
}



