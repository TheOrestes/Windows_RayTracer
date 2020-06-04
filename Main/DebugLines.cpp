
#include "DebugLines.h"
#include "GLLine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
void DebugLines::AddToLines(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
{	
	GLLine* line = new GLLine(start, end, color);
	m_vecDebugLines.push_back(line);
}

void DebugLines::InitializeLines()
{
	std::vector<GLLine*>::iterator iter = m_vecDebugLines.begin();

	for (; iter != m_vecDebugLines.end(); ++iter)
	{
		(*iter)->Init();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DebugLines::RenderLines()
{
	std::vector<GLLine*>::iterator iter = m_vecDebugLines.begin();

	for (; iter != m_vecDebugLines.end(); ++iter)
	{
		(*iter)->Render();
	}
}
