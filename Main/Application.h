#pragma once

#include <mutex>
#include <atomic>
#include <vector>

#include "GLFW\glfw3.h"
#include "glm/glm.hpp"

class Ray;
class Camera;
class ScreenAlignedQuad;

class Application
{
public:
	Application();
	~Application();

	void				Initialize(bool _threaded);
	void				Execute(GLFWwindow* window);
	void				UpdateGL(GLFWwindow* window);
	void				SaveImage();

	inline int			GetBufferWidth() { return m_iBackbufferWidth; }
	inline int			GetBufferHeight() { return m_iBackbufferHeight; }
	inline double		GetTotalRenderTime() { return m_dTotalRenderTime; }

private:
	glm::vec3			TraceColor(const Ray& r, int depth, int& rayCount);
	void				ShowProgress(int percentage);
	void				ParallelTrace(std::mutex* threadMutex, int i, GLFWwindow* window);
	void				Trace(GLFWwindow* window);


	int					m_iBackbufferWidth;
	int					m_iBackbufferHeight;
	int					m_iNumSamples;
	int					m_iMaxThreads;
	double				m_dTotalRenderTime;
	bool				m_bThreaded;

	std::atomic<uint64_t>	m_iRayCount;
	std::atomic<uint64_t>    m_iRayTriangleQuery;
	std::atomic<uint64_t>    m_iRayTriangleSuccess;
	std::atomic<uint64_t>    m_iRayBoxQuery;
	std::atomic<uint64_t>	m_iRayBoxSuccess;
	std::atomic<uint64_t>    m_iTriangleCount;

	Camera*				m_pCamera;

	ScreenAlignedQuad*	m_pQuad;

	std::vector<glm::vec3>	copyBuffer;
	std::vector<glm::vec3>  vecBuffer;
};
