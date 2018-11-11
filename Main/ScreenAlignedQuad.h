#pragma once

#include "GL\glew.h"
#include "GLSLShader.h"
#include "VertexStructures.h"

class ScreenAlignedQuad
{
public:
	ScreenAlignedQuad();
	~ScreenAlignedQuad();

	void Init(int width, int height);
	void UpdateTexture(int xStart, int yStart, int width, int height, float* pixels);
	void Render();
	void Kill();

private:
	GLuint		vao;
	GLuint		vbo;
	GLuint		tbo;

	GLint		posAttrib;
	GLint		texAttrib;

	VertexPT	quadVertices[6];
	GLSLShader* m_pFXShader;

	GLint		hScreenTexture;
};




