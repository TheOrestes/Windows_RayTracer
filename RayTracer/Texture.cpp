
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"

#include <iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////
ImageTexture::ImageTexture(const std::string & _path)
{
	path = _path;
	LoadImage();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 ImageTexture::value(glm::vec2 uv) const
{
	// Images with alpha channels not supported yet!
	if (channels > 3)
		return glm::vec3(1, 0, 0.8f);

	int i = (uv[0]) * width;
	int j = (1 - uv[1]) * height;

	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > width - 1) i = width - 1;
	if (j > height - 1) j = height - 1;

	float r = int(data[channels * i + channels * width * j]    ) / 255.0f;
	float g = int(data[channels * i + channels * width * j + 1]) / 255.0f;
	float b = int(data[channels * i + channels * width * j + 2]) / 255.0f;

	return glm::vec3(r, g, b);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void ImageTexture::LoadImage()
{
	data = stbi_load(path.c_str(), &width, &height, &channels, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HDRITexture::HDRITexture()
{
	data = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
HDRITexture::HDRITexture(const std::string& _path)
{
	path = _path;
	data = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void HDRITexture::LoadImage()
{
	if (stbi_is_hdr(path.c_str()))
	{
		stbi_set_flip_vertically_on_load(1);
		data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			std::cout << "Error loading HDRI image data!" << std::endl;
		}
	}
	else
	{
		std::cout << "Image not HDRI!" << std::endl;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
glm::vec3 HDRITexture::value(glm::vec2 uv) const
{
	return glm::vec3(1, 1, 1);
}
