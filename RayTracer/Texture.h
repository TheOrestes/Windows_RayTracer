#pragma once

#include "glm/glm.hpp"
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
class Texture
{
public:
	virtual glm::vec3 value(glm::vec2 uv) const = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////////
class CheckeredTexture : public Texture
{
public:
	CheckeredTexture() {}
	CheckeredTexture(glm::vec3 _col1, glm::vec3 _col2, float _valueX, float _valueY)
		: color1(_col1), color2(_col2), valX(_valueX), valY(_valueY) {}

	virtual glm::vec3 value(glm::vec2 uv) const
	{
		glm::vec2 finalUV = glm::vec2(0);

		finalUV.x = std::floorf(uv.x * valX);
		finalUV.y = std::floorf(uv.y * valY);

		float result = std::fmod(finalUV.x + finalUV.y, 2.0f);

		return ((1.0f - result) * color1 + result * color2);
	}

private:
	glm::vec3 color1;
	glm::vec3 color2;
	float valX;
	float valY;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
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
