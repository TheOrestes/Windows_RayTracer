
#pragma once

#include "GL/glew.h"
#include <string>
#include "glm/glm.hpp"

//////////////////////////////////////////////////////////////////////////////////////////
struct VertexP
{
	VertexP() : position(0.0f){}
	VertexP(glm::vec3 _p) : position(_p){}

	glm::vec3 position;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct VertexPC
{
	VertexPC() : 
		position(0.0f),
		color(1.0f){}

	VertexPC(const glm::vec3& _p, const glm::vec4& _c) : 
		position(_p),
		color(_c) {}

	glm::vec3	position;
	glm::vec4	color;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct VertexPT
{
	VertexPT() :
		position(0.0f),
		uv(0.0f) {}

	VertexPT(const glm::vec3& _p, const glm::vec2& _uv) :
		position(_p),
		uv(_uv) {}

	glm::vec3 position;
	glm::vec2 uv;
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
		position = glm::vec3(0);
		normal = glm::vec3(0);
		uv = glm::vec2(0);
	}

	VertexPNT(const glm::vec3& _pos, const glm::vec3& _normal, const glm::vec2 _uv) :
		position(_pos),
		normal(_normal),
		uv(_uv) {}

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

//////////////////////////////////////////////////////////////////////////////////////////
struct VertexPTNBT
{
	VertexPTNBT() :
		position(0.0f),
		uv(0.0f),
		normal(1.0f),
		binormal(0.0f),
		tangent(0.0f) {}

	VertexPTNBT(const glm::vec3& _p, const glm::vec3& _uv, const glm::vec3& _n, const glm::vec3& _b, const glm::vec3& _t ) :
		position(_p),
		uv(_uv),
		normal(_n),
		binormal(_b),
		tangent(_t){}

	glm::vec3	position;
	glm::vec2	uv;
	glm::vec3	normal;
	glm::vec3   tangent;
	glm::vec3   binormal;
};


