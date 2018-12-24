#pragma once

#include "glm/glm.hpp"

/////////////////////////////////////////////////////////////////////////////////////////
struct VertexP
{
	VertexP() {}
	VertexP(const glm::vec3& _pos) : position(_pos) {}

	glm::vec3 position;
};

/////////////////////////////////////////////////////////////////////////////////////////
struct VertexPN
{
	VertexPN() {}
	VertexPN(const glm::vec3& _pos, const glm::vec3& _normal) :	
		position(_pos),
		normal(_normal) {}

	glm::vec3 position;
	glm::vec3 normal;
};

/////////////////////////////////////////////////////////////////////////////////////////
struct VertexPNT
{
	VertexPNT() {}
	VertexPNT(const glm::vec3& _pos, const glm::vec3& _normal, const glm::vec3 _uv) :
		position(_pos),
		normal(_normal),
		uv(_uv) {}

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 uv;
};
