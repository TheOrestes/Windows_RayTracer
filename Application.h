#pragma once

#include "glm/glm.hpp"
#include "OpenImageDenoise/oidn.hpp"
#include <Windows.h>
#include <mutex>
#include <atomic>
#include <vector>

class Ray;
class Scene;

class Application
{
public:
	Application();
	~Application();

	void			Initialize(HWND hwnd, bool _threaded);
	void			Execute();
	void			SaveImage();
	void			DenoiseImage();

	inline int		GetBufferWidth() { return m_iBackbufferWidth; }
	inline int		GetBufferHeight() { return m_iBackbufferHeight; }
	inline float	GetTotalRenderTime() { return m_dTotalRenderTime; }

private:
	glm::vec3		TraceColor(const Ray& r, int depth, int& rayCount);
	void			ShowProgress(int percentage);
	void			ParallelTrace(std::mutex* threadMutex, int i);
	void			Trace();

	int				m_iBackbufferWidth;
	int				m_iBackbufferHeight;
	int				m_iNumSamples;
	int				m_iMaxThreads;
	float			m_dTotalRenderTime;
	float			m_dDenoiserTime;
	bool			m_bThreaded;

	std::atomic<uint64_t>	m_iRayCount;
	std::atomic<uint64_t>    m_iRayTriangleQuery;
	std::atomic<uint64_t>    m_iRayTriangleSuccess;
	std::atomic<uint64_t>    m_iRayBoxQuery;
	std::atomic<uint64_t>	m_iRayBoxSuccess;
	std::atomic<uint64_t>    m_iTriangleCount;

	HWND			m_hWnd;
	Scene*			m_pScene;

	oidn::DeviceRef	m_oidnDevice;
	oidn::FilterRef m_oidnFilter;
};
