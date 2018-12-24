#pragma once

#include "glm/glm.hpp"
#include <string>

class Texture
{
public:
	virtual glm::vec3 value(glm::vec2 uv) const = 0;
};

class ConstantTexture : public Texture
{
public:
	ConstantTexture() {}
	ConstantTexture(glm::vec3 col) : color(col) {}

	virtual glm::vec3 value(glm::vec2 uv) const
	{
		return color;
	}

private:
	glm::vec3 color;
};

class ImageTexture : public Texture
{
public:
	ImageTexture() {}
	ImageTexture(const std::string& _path);

	virtual glm::vec3 value(glm::vec2 uv) const;

private:
	glm::vec3 color;
	std::string path;

	void LoadImage();

	int width, height, channels;
	unsigned char* data;
};