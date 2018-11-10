
#include "ScreenAlignedQuad.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenAlignedQuad::ScreenAlignedQuad()
{
	m_pFXShader = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenAlignedQuad::~ScreenAlignedQuad()
{
	Kill();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenAlignedQuad::Init(int width, int height)
{
	// Create Shader that will used to draw screen aligned quad...
	m_pFXShader = new GLSLShader("Main/vsScreenQuad.glsl", "Main/psScreenQuad.glsl");
	if (m_pFXShader && m_pFXShader->GetShaderID() != -1)
	{
		hScreenTexture = glGetUniformLocation(m_pFXShader->GetShaderID(), "screenTexture");
	}

	// Create screen aligned quad data in NDC space.
	quadVertices[0] = VertexPT(glm::vec3(-1, 1, 0), glm::vec2(0, 1));
	quadVertices[1] = VertexPT(glm::vec3(-1, -1, 0), glm::vec2(0, 0));
	quadVertices[2] = VertexPT(glm::vec3(1, -1, 0), glm::vec2(1, 0));
	quadVertices[3] = VertexPT(glm::vec3(-1, 1, 0), glm::vec2(0, 1));
	quadVertices[4] = VertexPT(glm::vec3(1, -1, 0), glm::vec2(1, 0));
	quadVertices[5] = VertexPT(glm::vec3(1, 1, 0), glm::vec2(1, 1));

	// Create VBO & VAO for the screen aligned quad...
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (void*)offsetof(VertexPT, uv));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// create texture buffer object
	glGenTextures(1, &tbo);
	glBindTexture(GL_TEXTURE_2D, tbo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenAlignedQuad::UpdateTexture(int xStart, int yStart, int width, int height, const float* pixels)
{
	glBindTexture(GL_TEXTURE_2D, tbo);

	glTexSubImage2D(GL_TEXTURE_2D, 0, xStart, yStart, width, height, GL_RGB, GL_FLOAT, (void*)pixels);

	glBindTexture(GL_TEXTURE_2D, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenAlignedQuad::Render()
{
	glDisable(GL_DEPTH_TEST);

	m_pFXShader->Use();
	glUniform1i(hScreenTexture, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tbo);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenAlignedQuad::Kill()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &tbo);

	delete m_pFXShader;
	m_pFXShader = nullptr;

	delete[] quadVertices;
}



