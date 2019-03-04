
#include "Triangle.h"

//#define MOLLER_TRUMBORE

/////////////////////////////////////////////////////////////////////////////////////////
bool Triangle::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	++rec.rayTriangleTestCount;

	glm::vec3 rayDirection = r.GetRayDirection();
	glm::vec3 rayOrigin = r.GetRayOrigin();

	// Compute Plane Normal
	glm::vec3 edge0 = v1.position - v0.position;
	glm::vec3 edge1 = v2.position - v1.position;
	glm::vec3 edge2 = v0.position - v2.position;

	glm::vec3 barycentric;

#ifdef MOLLER_TRUMBORE
	glm::vec3 pvec = glm::cross(rayDirection, edge2);
	float det = glm::dot(edge0, pvec);

	if (fabs(det) < 0.0001f) 
		return false;

	float invDet = 1 / det;

	glm::vec3 tvec = rayOrigin - v0.position;
	barycentric.x = glm::dot(tvec, pvec) * invDet;

	if (barycentric.x < 0 || barycentric.x > 1)
		return false;

	glm::vec3 qvec = glm::cross(tvec, edge0);
	barycentric.y = glm::dot(rayDirection, qvec) * invDet;

	if (barycentric.y < 0 || barycentric.x + barycentric.y > 1)
		return false;

	barycentric.z = 1 - barycentric.x - barycentric.y; //glm::dot(edge2, qvec) * invDet;

	// Got barycentric, now calculate P,N & UV coordinates
	// Record hit data!!!
	rec.t = 100.0f;
	rec.P = v0.position * barycentric.x + v1.position * barycentric.y + v2.position * barycentric.z;
	rec.N = v0.normal * barycentric.x +   v1.normal * barycentric.y +   v2.normal * barycentric.z;
	rec.uv =v0.uv * barycentric.x +       v1.uv * barycentric.y +       v2.uv * barycentric.z;
	rec.mat_ptr = mat_ptr;

	return true;
#else
	// NOTE that we are not normalizing the normal vector
	// as we need to take it's area.
	// cross product's magnitude is area of parallelogram formed by two vectors
	glm::vec3 area = glm::cross(edge0, edge1);
	float areaOfParellogram = glm::length(area);

	// Normalize normal now!
	// cross product's vector direction represents new vector perpendicular to 
	// plane formed by those two vectors!
	glm::vec3 N = glm::normalize(area);

	// Check if ray & plane are parallel
	float NDotRayDirection = glm::dot(N, rayDirection);
	if (fabs(NDotRayDirection) < 0.001f)
		return false;

	// Compute plane distance from origin
	float d = glm::dot(v0.position, N);

	// Compute t at which intersection happens!
	float t = (d - glm::dot(N, rayOrigin)) / NDotRayDirection;

	if (t < tmin || t > tmax)
		return false;

	// Compute intersection point
	glm::vec3 P = rayOrigin + (t * rayDirection);

	// Perform tests if this P is inside triangle or outside
	glm::vec3 C;
	glm::vec3 P0 = P - v0.position;
	glm::vec3 P1 = P - v1.position;
	glm::vec3 P2 = P - v2.position;

	glm::vec3 C0 = glm::cross(edge0, P0);
	glm::vec3 C1 = glm::cross(edge1, P1);
	glm::vec3 C2 = glm::cross(edge2, P2);

	if (glm::dot(N, C0) >= 0 && glm::dot(N, C1) >= 0 && glm::dot(N, C2) >= 0)
	{
		float length0 = glm::length(C0);
		float length1 = glm::length(C1);
		barycentric.x = length0 / areaOfParellogram;
		barycentric.y = length1 / areaOfParellogram;
		barycentric.z = 1 - barycentric.x - barycentric.y;

		// Record hit data!!!
		rec.t = t;
		rec.P = P;
		rec.N = N;
		rec.uv = barycentric.x * v2.uv + barycentric.y * v0.uv + barycentric.z * v1.uv;
		rec.mat_ptr = mat_ptr;

		++rec.rayTriangleIntersectionCount;

		return true;
	}
	else
		return false;
#endif
	

	//auto edge0 = V1 - V0;
	//auto edge1 = V2 - V1;
	//auto normal = UnitVector(Cross(edge0, edge1));
	//auto planeOffset = glm::dot(V0, normal);
	//auto p0 = r.GetPointAt(tmin);
	//auto p1 = r.GetPointAt(tmax);
	//auto offset0 = glm::dot(p0, normal);
	//auto offset1 = glm::dot(p1, normal);
	//if ((offset0 - planeOffset)*(offset1 - planeOffset) <= 0.f) // Line segment intersects the plane of the triangle
	//{
	//	float t = tmin + (tmax - tmin)*(planeOffset - offset0) / (offset1 - offset0);
	//	auto p = r.GetPointAt(t);
	//	auto c0 = Cross(edge0, p - V0);
	//	auto c1 = Cross(edge1, p - V1);
	//	if (glm::dot(c0, c1) >= 0.f)
	//	{
	//		auto edge2 = V0 - V2;
	//		auto c2 = Cross(edge2, p - V2);
	//		if (glm::dot(c1, c2) >= 0.f)
	//		{
	//			rec.t = t;
	//			rec.P = p;
	//			rec.N = normal;
	//			rec.mat_ptr = mat_ptr;
	//			return true;
	//		}
	//	}
	//}
	//return false;
}