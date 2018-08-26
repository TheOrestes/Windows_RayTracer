#pragma once

#include <math.h>
#include <stdlib.h>
#include <iostream>

class Vector3
{
public:
	Vector3() {}
	Vector3(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }

	inline const Vector3& operator+() const { return *this; }
	inline Vector3 operator-() { return Vector3(-x, -y, -z); }

	inline Vector3& operator+=(const Vector3& v2);
	inline Vector3& operator-=(const Vector3& v2);
	inline Vector3& operator*=(const Vector3& v2);
	inline Vector3& operator/=(const Vector3& v2);
	inline Vector3& operator*=(const float t);
	inline Vector3& operator/=(const float t);

	inline float length() const { return sqrt(x * x + y * y + z * z); }
	inline float squaredLength() const { return(x*x + y * y + z * z); }
	
	float x, y, z;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& os, const Vector3& v)
{
	os << v.x << " " << v.y << " " << v.z;
	return os;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline std::istream& operator>>(std::istream &is, Vector3 &v) 
{
	is >> v.x >> v.y >> v.z;
	return is;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline Vector3 operator*(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline Vector3 operator/(const Vector3& v1, const Vector3& v2)
{
	return Vector3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline Vector3 operator*(float t, const Vector3& v2)
{
	return Vector3(t * v2.x, t * v2.y, t * v2.z);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline Vector3 operator/(const Vector3& v2, float t)
{
	return Vector3(v2.x / t, v2.y / t, v2.z / t);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline float dot(const Vector3& v1, const Vector3& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline Vector3 cross(const Vector3& v1, const Vector3& v2)
{
	return Vector3((v1.y*v2.z - v1.z*v2.y), (-(v1.x*v2.z - v1.z*v2.x)), (v1.x*v2.y-v1.y*v2.x));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inline Vector3 unit_vector(Vector3 v)
{
	return v / v.length();
}



