
#include "Triangle.h"

/////////////////////////////////////////////////////////////////////////////////////////
bool Triangle::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	// Compute Plane Normal
	glm::vec3 edge0 = V1 - V0;
	glm::vec3 edge1 = V2 - V1;
	glm::vec3 edge2 = V0 - V2;

	glm::vec3 N = glm::normalize(glm::cross(edge0, edge1));

	glm::vec3 rayDirection = r.GetRayDirection();
	glm::vec3 rayOrigin = r.GetRayOrigin();

	// Check if ray & plane are parallel
	float NDotRayDirection = glm::dot(N, rayDirection); 
	if (fabs(NDotRayDirection) < 0.001f)
		return false;

	// Compute plane distance from origin
	float d = glm::dot(V0, N); 

	// Compute t at which intersection happens!
	float t = (d - glm::dot(N, rayOrigin)) / NDotRayDirection;

	if (t < tmin || t > tmax)
		return false;

	// Compute intersection point
	glm::vec3 P = rayOrigin + (t * rayDirection);

	// Perform tests if this P is inside triangle or outside
	glm::vec3 C;
	glm::vec3 P0 = P - V0;
	glm::vec3 P1 = P - V1;
	glm::vec3 P2 = P - V2;

	if (glm::dot(N, glm::cross(edge0, P0)) >= 0 && glm::dot(N, glm::cross(edge1, P1)) >= 0 && glm::dot(N, glm::cross(edge2, P2)) >= 0)
	{
		// Record hit data!!!
		rec.t = t;
		rec.P = P;
		rec.N = N;
		rec.mat_ptr = mat_ptr;

		return true;
	}
	else
		return false;

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