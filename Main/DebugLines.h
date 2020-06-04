#pragma once

#include <vector>
#include "glm/glm.hpp"

class GLLine;

class DebugLines
{
public:
	DebugLines() { m_vecDebugLines.clear(); }
	~DebugLines() { m_vecDebugLines.clear(); }

	void	AddToLines(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
	void	InitializeLines();
	void	RenderLines();

private:
	std::vector<GLLine*> m_vecDebugLines;
};


