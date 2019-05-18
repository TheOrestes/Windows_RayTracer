#pragma once

#include "glm/glm.hpp"

/////////////////////////////////////////////////////////////////////////////////////////
struct VertexP
{
	VertexP() { position = glm::vec3(0); }
	VertexP(const glm::vec3& _pos) : position(_pos) {}

	glm::vec3 position;
};

/////////////////////////////////////////////////////////////////////////////////////////
struct VertexPN
{
	VertexPN()
	{ 
		position = glm::vec3(0);
		normal = glm::vec3(0);
	}

	VertexPN(const glm::vec3& _pos, const glm::vec3& _normal) :	
		position(_pos),
		normal(_normal) {}

	glm::vec3 position;
	glm::vec3 normal;
};

/////////////////////////////////////////////////////////////////////////////////////////
struct VertexPNT
{
	VertexPNT() 
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		normal =   glm::vec3(0.0f, 0.0f, 0.0f);
		uv = glm::vec2(0.0f, 0.0f);
	}

	VertexPNT(const glm::vec3& _pos, const glm::vec3& _normal, const glm::vec2 _uv) :
		position(_pos),
		normal(_normal),
		uv(_uv) {}

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};
