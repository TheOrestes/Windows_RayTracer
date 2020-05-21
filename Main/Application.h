#pragma once

#include <mutex>
#include <atomic>
#include <vector>

#include "GLFW\glfw3.h"
#include "glm/glm.hpp"
#include "OpenImageDenoise/oidn.hpp"

class Ray;
class ScreenAlignedQuad;
class Scene;
class Sampler;

class Application
{
public:
	Application();
	~Application();

	void					Initialize(bool _threaded);
	void					Execute(GLFWwindow* window);
	void					UpdateGL(GLFWwindow* window);
	void					SaveImage();
	void					DenoiseImage();

	void					RenderPixel(int rowIndex, int columnIndex);
	void					RenderDebugPixel(int rowIndex, int columnIndex);

	inline int				GetBufferWidth() { return m_iBackbufferWidth; }
	inline int				GetBufferHeight() { return m_iBackbufferHeight; }
	inline double			GetTotalRenderTime() { return m_dTotalRenderTime; }

private:
	glm::vec3				TraceColor(const Ray& r, int depth, int& rayCount);
	void					ShowProgress(int percentage);
	void					ParallelTrace(std::mutex* threadMutex, int i, GLFWwindow* window);
	void					Trace(GLFWwindow* window);

	uint32_t				m_iBackbufferWidth;
	uint32_t				m_iBackbufferHeight;
	uint32_t				m_iNumSamples;
	int						m_iMaxThreads;
	double					m_dTotalRenderTime;
	float					m_dDenoiserTime;
	bool					m_bThreaded;

	std::atomic<uint64_t>	m_iRayCount;
	std::atomic<uint64_t>   m_iRayTriangleQuery;
	std::atomic<uint64_t>   m_iRayTriangleSuccess;
	std::atomic<uint64_t>   m_iRayBoxQuery;
	std::atomic<uint64_t>	m_iRayBoxSuccess;
	std::atomic<uint64_t>   m_iTriangleCount;

	ScreenAlignedQuad*		m_pQuad;
	Sampler*				m_pSampler;

	std::vector<glm::vec3>  m_vecSrcPixels;
	std::vector<glm::vec3>	m_vecDstPixels;

	oidn::DeviceRef			m_oidnDevice;
	oidn::FilterRef			m_oidnFilter;
};
