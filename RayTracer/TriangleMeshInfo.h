#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
struct Transform
{
	Transform()
	{
		matWorld = glm::mat4(1);
		matInvWorld = glm::mat4(1);
		matInvTransposeWorld = glm::mat4(1);
	}

	Transform(const glm::vec3& _pos, const glm::vec3& _axis, float _angle, const glm::vec3& _scale)
	{
		// Identity matrices...
		matWorld = glm::mat4(1);
		matInvWorld = glm::mat4(1);
		matInvTransposeWorld = glm::mat4(1);

		// World matrix
		matWorld = glm::translate(matWorld, _pos);
		matWorld = glm::rotate(matWorld, _angle, _axis);
		matWorld = glm::scale(matWorld, _scale);

		// World inverse & world inverse transpose
		matInvWorld = glm::inverse(matWorld);
		matInvTransposeWorld = glm::transpose(matInvWorld);
	}

	glm::mat4 matWorld;
	glm::mat4 matInvWorld;
	glm::mat4 matInvTransposeWorld;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
struct MaterialInfo
{
	MaterialInfo()
	{
		albedoColor = glm::vec4(0);		// logic is dependent on this being 0 i.e. if length(albedoColor) == 0 then we use Maya's color!
		roughness = 1.0f;
		refrIndex = 1.0f;
	}

	glm::vec4		albedoColor;
	float			roughness;
	float			refrIndex;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
struct MeshInfo
{
	MeshInfo()
	{
		filePath.clear();
		position = glm::vec3(0);
		rotationAxis = glm::vec3(1);
		rotationAngle = 0.0f;
		scale = glm::vec3(1);
		leafSize = 1;
		isLightSource = false;
	}

	std::string		filePath;

	// Transform...
	glm::vec3		position;
	glm::vec3		rotationAxis;
	float			rotationAngle;
	glm::vec3		scale;

	// Material
	MaterialInfo    matInfo;

	// BVH related...
	uint32_t		leafSize;

	// Misc...
	bool			isLightSource;
};
