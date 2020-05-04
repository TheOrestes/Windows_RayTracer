#pragma once

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"
#include <cstdlib>
#include <time.h>
#include <string>

const float PI = 3.14159265358f;
const float TWO_PI = 6.283185307179f;
const float PI_OVER_TWO = 1.570796326794f;
const float INV_PI = 0.3183098861837f;

namespace Helper
{
	inline std::string GetCurrentDateTime()
	{
		time_t now = time(0);
		struct tm tstruct;
		char buf[80];

		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%d_%B_%Y_%H_%M_%S", &tstruct);

		return buf;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 LerpVector(const glm::vec3& vec1, const glm::vec3& vec2, float t)
	{
		return (1.0f - t) * vec1 + t * vec2;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline float GetRandom01()
	{
		return ((float)rand() / (RAND_MAX + 1));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 GetRandomInUnitDisk()
	{
		glm::vec3 p;
		do
		{
			p = 2.0f * glm::vec3(GetRandom01(), GetRandom01(), 0.0f) - glm::vec3(1, 1, 0);
		} while (glm::dot(p, p) >= 1.0f);
		
		return p;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 RandomInUnitSphere()
	{
		glm::vec3 P;

		do
		{
			P = 2.0f * glm::vec3(GetRandom01(), GetRandom01(), GetRandom01()) - glm::vec3(1, 1, 1);
		} while (glm::length2(P) >= 1.0f);

		return P;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 CosineSamplingUpperHemisphere(glm::vec3 Normal)
	{
		float rand1 = GetRandom01();
		float rand2 = GetRandom01();
		
		float phi = TWO_PI * rand2;	// phi = 2PI * esp2
		float y = sqrtf(rand1);		// y = sqrt(eps1)

		float theta = acosf(y);

		// Generate random sample
		float X = sinf(theta) * cosf(phi);
		float Y = cosf(theta);
		float Z = sinf(theta) * sinf(phi);

		//return glm::vec3(X, Y, Z);

		// This sample is oriented towards Local Y axis instead of oriented as per the hitpoint normal
		// need to do that before actually using it!
		glm::vec3 Up;
		if (fabsf(Normal.y > 0.9f))
			Up = glm::vec3(1, 0, 0);
		else
			Up = glm::vec3(0, 1, 0);

		glm::vec3 v = glm::normalize(Normal);
		glm::vec3 u = glm::normalize(glm::cross(Up, v));
		glm::vec3 w = glm::cross(v, u);
		
		return X * u + Y * v + Z * w;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 PhongImportanceSampling(glm::vec3 Normal, glm::vec3 Reflection, float Ks, float SpecularPower)
	{
		float rand1 = GetRandom01();
		float rand2 = GetRandom01();

		float theta = acosf(powf(rand1, (1.0f / (SpecularPower + 1.0f))));
		float phi = rand2 * TWO_PI;

		// Generate random sample
		float X = sinf(theta) * cosf(phi);
		float Y = cosf(theta);
		float Z = sinf(theta) * sinf(phi);

		//return glm::vec3(X, Y, Z);

		// This sample is oriented towards Local Y axis instead of oriented as per the reflection vector
		// need to do that before actually using it!
		glm::vec3 Up;
		if (fabsf(Reflection.y > 0.9f))
			Up = glm::vec3(1, 0, 0);
		else
			Up = glm::vec3(0, 1, 0);

		glm::vec3 v = glm::normalize(Reflection);
		glm::vec3 u = glm::normalize(glm::cross(Up, v));
		glm::vec3 w = glm::cross(v, u);

		return X * u + Y * v + Z * w;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 ModifiedPhongImportanceSampling(glm::vec3 Normal, glm::vec3 Reflection, float Ks, float SpecularPower)
	{
		float rand1 = GetRandom01();
		float rand2 = GetRandom01();
		float rand3 = GetRandom01();

		float phi = rand3 * TWO_PI;

		if (rand1 < 1-Ks)
		{
			// Diffuse
			float theta = acosf(sqrtf(rand2));

			// Generate random sample
			float X = sinf(theta) * cosf(phi);
			float Y = cosf(theta);
			float Z = sinf(theta) * sinf(phi);

			// This sample is oriented towards Local Y axis instead of oriented as per the normal vector
			// need to do that before actually using it!
			glm::vec3 Up;
			if (fabsf(Normal.y > 0.9f))
				Up = glm::vec3(1, 0, 0);
			else
				Up = glm::vec3(0, 1, 0);

			glm::vec3 v = glm::normalize(Normal);
			glm::vec3 u = glm::normalize(glm::cross(Up, v));
			glm::vec3 w = glm::cross(v, u);

			return X * u + Y * v + Z * w;
		}
		else
		{
			// Specular
			float theta = acosf(powf(rand2, (1.0f / (SpecularPower + 1.0f))));

			// Generate random sample
			float X = sinf(theta) * cosf(phi);
			float Y = cosf(theta);
			float Z = sinf(theta) * sinf(phi);

			// This sample is oriented towards Local Y axis instead of oriented as per the reflection vector
			// need to do that before actually using it!
			glm::vec3 Up;
			if (fabsf(Reflection.y > 0.9f))
				Up = glm::vec3(1, 0, 0);
			else
				Up = glm::vec3(0, 1, 0);

			glm::vec3 v = glm::normalize(Reflection);
			glm::vec3 u = glm::normalize(glm::cross(Up, v));
			glm::vec3 w = glm::cross(v, u);

			return X * u + Y * v + Z * w;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 RandomUnitVector()
	{
		float z = GetRandom01() * 2.0f - 1.0f;
		float a = GetRandom01() * 2.0f * PI;
		float r = sqrtf(1.0f - z * z);
		float x = r * cosf(a);
		float y = r * sinf(a);

		return glm::vec3(x, y, z);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline glm::vec3 Reflect(const glm::vec3& dir, const glm::vec3& normal)
	{
		return (dir - 2.0f * glm::dot(dir, normal) * normal);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline bool Refract(const glm::vec3& v, const glm::vec3& n, float ni_over_nt, glm::vec3& refracted)
	{
		glm::vec3 unit_v = glm::normalize(v);
		float NDotV = glm::dot(unit_v, n);
		float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1 - NDotV * NDotV);

		if (discriminant > 0)
		{
			refracted = ni_over_nt * (unit_v - NDotV * n) - sqrt(discriminant) * n;
			return true;
		}
		else
			return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline float schlick(float cosine, float ref_idx)
	{
		float r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0)*pow((1 - cosine), 5);
	}
}
