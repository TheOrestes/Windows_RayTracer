#pragma once

#include "Vector3.h"

const float PI = 3.14159265358f;

namespace Helper
{
	inline Vector3 LerpVector(const Vector3& vec1, const Vector3& vec2, float t)
	{
		return (1.0f - t) * vec1 + t * vec2;
	}

	inline double GetRandom01()
	{
		return ((double)rand() / (RAND_MAX + 1));
	}

	inline Vector3 GetRandomInUnitDisk()
	{
		Vector3 p;
		do
		{
			p = 2.0f * Vector3(GetRandom01(), GetRandom01(), 0.0f) - Vector3(1, 1, 0);
		} while (dot(p, p) >= 1.0f);
		
		return p;
	}

	inline Vector3 RandomInUnitSphere()
	{
		Vector3 P;

		do
		{
			P = 2.0f * Vector3(GetRandom01(), GetRandom01(), GetRandom01()) - Vector3(1, 1, 1);
		} while (P.squaredLength() >= 1.0f);

		return P;
	}

	inline Vector3 Reflect(const Vector3& v, const Vector3& n)
	{
		return v - 2 * dot(v, n) * n;
	}

	inline bool Refract(const Vector3& v, const Vector3& n, float ni_over_nt, Vector3& refracted)
	{
		Vector3 unit_v = unit_vector(v);
		float NdotV = dot(unit_v, n);
		float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - NdotV * NdotV);

		if (discriminant > 0)
		{
			refracted = ni_over_nt * (unit_v - NdotV * n) - sqrt(discriminant) * n;
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
