
#include "GLLine.h"
#include "GLSLShader.h"
#include "../RayTracer/Camera.h"

#include "glm/gtc/type_ptr.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
GLLine::GLLine()
{
	// Vertex Data
	m_vertices[0] = VertexPC(glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1));
	m_vertices[1] = VertexPC(glm::vec3(1, 1, 1), glm::vec4(1, 1, 1, 1));
}

//////////////////////////////////////////////////////////////////////////////////////////
GLLine::GLLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
{
	// Vertex Data
	m_vertices[0] = VertexPC(start, color);
	m_vertices[1] = VertexPC(end, color);
}

//////////////////////////////////////////////////////////////////////////////////////////
GLLine::~GLLine()
{
	delete m_pShader;

	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLLine::Init()
{
	// Create shader object
	m_pShader = new GLSLShader("Main/vsLine.glsl", "Main/psLine.glsl");

	// create vao
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// create vbo
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(VertexPC), m_vertices, GL_STATIC_DRAW);

	GLuint shader = m_pShader->GetShaderID();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexPC), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(VertexPC), (void*)offsetof(VertexPC, color));

	SetupViewProjectionMatrix();

	//glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLLine::SetupViewProjectionMatrix()
{
	m_matView = glm::lookAt(Camera::getInstance().position, Camera::getInstance().lookAt, Camera::getInstance().Up);

	// tan(fov/2) = ( 0.5 * screenHeight ) / d;
	float fov = (2.0f * atanf((0.5f * Camera::getInstance().screenHeight) / Camera::getInstance().viewPlaneDistance));
	m_matProj = glm::perspective(fov, Camera::getInstance().screenWidht / Camera::getInstance().screenHeight, 0.1f, 1000.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////
void GLLine::Render()
{
	glBindVertexArray(m_vao);

	m_pShader->Use();

	GLuint shader = m_pShader->GetShaderID();
	m_hView = glGetUniformLocation(shader, "matView");
	m_hProj = glGetUniformLocation(shader, "matProj");

	glUniformMatrix4fv(m_hView, 1, GL_FALSE, glm::value_ptr(m_matView));
	glUniformMatrix4fv(m_hProj, 1, GL_FALSE, glm::value_ptr(m_matProj));

	// know more about the last element!
	// http://stackoverflow.com/questions/17191258/no-display-from-gldrawelements
	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(0);
}

