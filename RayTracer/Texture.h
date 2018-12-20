#pragma once

#include "glm/vec3.hpp"

class Texture
{
public:
	virtual glm::vec3 value(float u, float v) const = 0;
};

class ConstantTexture : public Texture
{
public:
	ConstantTexture() {}
	ConstantTexture(glm::vec3 col) : color(col) {}

	virtual glm::vec3 value(float u, float v) const
	{
		return color;
	}

private:
	glm::vec3 color;
};