
#include "Triangle.h"

/////////////////////////////////////////////////////////////////////////////////////////
bool Triangle::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	// Compute Plane Normal
	Vector3 edge0 = V1 - V0;
	Vector3 edge1 = V2 - V1;
	Vector3 edge2 = V0 - V2;

	Vector3 N = unit_vector(cross(edge0, edge1));

	// Check if ray & plane are parallel
	float NdotRayDirection = dot(N, r.GetRayDirection());
	if (fabs(NdotRayDirection) < 0.001f)
		return false;

	// Compute plane distance from origin
	float d = dot(V0, N);

	// Compute t at which intersection happens!
	float t = (d - dot(N, r.GetRayOrigin())) / NdotRayDirection;

	if (t < tmin || t > tmax)
		return false;

	// Compute intersection point
	Vector3 P = r.GetRayOrigin() + (t * r.GetRayDirection());

	// Perform tests if this P is inside triangle or outside
	Vector3 C;
	Vector3 P0 = P - V0;
	Vector3 P1 = P - V1;
	Vector3 P2 = P - V2;

	if (dot(N, cross(edge0, P0)) >= 0 && dot(N, cross(edge1, P1)) >= 0 && dot(N, cross(edge2, P2)) >= 0)
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
	//auto normal = unit_vector(cross(edge0, edge1));
	//auto planeOffset = dot(V0, normal);
	//auto p0 = r.GetPointAt(tmin);
	//auto p1 = r.GetPointAt(tmax);
	//auto offset0 = dot(p0, normal);
	//auto offset1 = dot(p1, normal);
	//if ((offset0 - planeOffset)*(offset1 - planeOffset) <= 0.f) // Line segment intersects the plane of the triangle
	//{
	//	float t = tmin + (tmax - tmin)*(planeOffset - offset0) / (offset1 - offset0);
	//	auto p = r.GetPointAt(t);
	//	auto c0 = cross(edge0, p - V0);
	//	auto c1 = cross(edge1, p - V1);
	//	if (dot(c0, c1) >= 0.f)
	//	{
	//		auto edge2 = V0 - V2;
	//		auto c2 = cross(edge2, p - V2);
	//		if (dot(c1, c2) >= 0.f)
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