
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture.h"

ImageTexture::ImageTexture(const std::string & _path)
{
	path = _path;
	LoadImage();
}

glm::vec3 ImageTexture::value(glm::vec2 uv) const
{
	// Images with alpha channels not supported yet!
	if (channels > 3)
		return glm::vec3(1, 0, 0.8f);

	int i = (uv.x) * width;
	int j = (1 - uv.y) * height;

	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > width - 1) i = width - 1;
	if (j > height - 1) j = height - 1;

	float r = int(data[channels * i + channels * width * j]    ) / 255.0f;
	float g = int(data[channels * i + channels * width * j + 1]) / 255.0f;
	float b = int(data[channels * i + channels * width * j + 2]) / 255.0f;

	return glm::vec3(r, g, b);
}

void ImageTexture::LoadImage()
{
	data = stbi_load(path.c_str(), &width, &height, &channels, 0);
}
