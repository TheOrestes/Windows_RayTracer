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

Application* pApp = new Application();
float lastX, lastY = 0.0f;

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

//////////////////////////////////////////////////////////////////////////
// Mouse Inputs
void MousePosHandler(GLFWwindow* window, double xPos, double yPos)
{
	float xoffset = xPos - lastX;
	float yoffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;
}

void MouseButtonClickHandler(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		pApp->RenderPixel(lastX, lastY);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	GLFWwindow* window;

	window = InitGLFW(pApp);
	if (!window)
		return 0;

	InitGLEW();

	glfwSetKeyCallback(window, KeyHandler);		
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(window, MousePosHandler);
	glfwSetMouseButtonCallback(window, MouseButtonClickHandler);

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



