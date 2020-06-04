#pragma once

#include "GL/glew.h"
#include "VertexStructures.h"

class GLSLShader;

class GLLine
{
public:
	GLLine();
	GLLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);

	~GLLine();

	void		Init();
	void		update(float dt);
	void		Render();

private:
	GLLine(const GLLine&);
	void operator=(const GLLine&);

	void		SetupViewProjectionMatrix();

	GLuint		m_vao;
	GLuint		m_vbo;

	GLint		m_posAttrib;
	GLint		m_colAttrib;
	VertexPC	m_vertices[2];

	GLint		m_hView;
	GLint		m_hProj;

	GLSLShader* m_pShader;

	glm::mat4	m_matView;
	glm::mat4	m_matProj;
};
