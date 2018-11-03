#pragma once

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"
#include <cstdlib>

const float PI = 3.14159265358f;

namespace Helper
{
	inline glm::vec3 LerpVector(const glm::vec3& vec1, const glm::vec3& vec2, float t)
	{
		return (1.0f - t) * vec1 + t * vec2;
	}

	inline double GetRandom01()
	{
		return ((double)rand() / (RAND_MAX + 1));
	}

	inline glm::vec3 GetRandomInUnitDisk()
	{
		glm::vec3 p;
		do
		{
			p = 2.0f * glm::vec3(GetRandom01(), GetRandom01(), 0.0f) - glm::vec3(1, 1, 0);
		} while (glm::dot(p, p) >= 1.0f);
		
		return p;
	}

	inline glm::vec3 RandomInUnitSphere()
	{
		glm::vec3 P;

		do
		{
			P = 2.0f * glm::vec3(GetRandom01(), GetRandom01(), GetRandom01()) - glm::vec3(1, 1, 1);
		} while (glm::length2(P) >= 1.0f);

		return P;
	}

	inline bool Refract(const glm::vec3& v, const glm::vec3& n, float ni_over_nt, glm::vec3& refracted)
	{
		glm::vec3 unit_v = glm::normalize(v);
		float NDotV = glm::dot(unit_v, n);
		float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - NDotV * NDotV);

		if (discriminant > 0)
		{
			refracted = ni_over_nt * (unit_v - NDotV * n) - sqrt(discriminant) * n;
			return true;
		}
		else
			return false;
	}

	inline float schlick(float cosine, float ref_idx)
	{
		float r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0)*pow((1 - cosine), 5);
	}
}
