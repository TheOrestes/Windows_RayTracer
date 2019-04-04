
#include "AABB.h"
#include "Triangle.h"

//#define MOLLER_TRUMBORE

Triangle::Triangle(const VertexPNT & _v0, const VertexPNT & _v1, const VertexPNT & _v2, Material * ptr_mat)
{
	v0 = _v0; v1 = _v1; v2 = _v2;
	mat_ptr = ptr_mat;

	// calculate centroid...
	centroid = v0.position + v1.position + v2.position;
	centroid /= 3.0f;
	//float x = (v0.position[0] + v1.position[0] + v2.position[0]) / 3.0f;
	//float y = (v0.position[1] + v1.position[1] + v2.position[1]) / 3.0f;
	//float z = (v0.position[2] + v1.position[2] + v2.position[2]) / 3.0f;
	//centroid = glm::vec3(x, y, z);
}

/////////////////////////////////////////////////////////////////////////////////////////
bool Triangle::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	++rec.rayTriangleQuery;

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
	barycentric[0] = glm::dot(tvec, pvec) * invDet;

	if (barycentric[0] < 0 || barycentric[0] > 1)
		return false;

	glm::vec3 qvec = glm::cross(tvec, edge0);
	barycentric[1] = glm::dot(rayDirection, qvec) * invDet;

	if (barycentric[1] < 0 || barycentric[0] + barycentric[1] > 1)
		return false;

	barycentric[2] = 1 - barycentric[0] - barycentric[1]; //glm::dot(edge2, qvec) * invDet;

	// Got barycentric, now calculate P,N & UV coordinates
	// Record hit data!!!
	rec.t = 100.0f;
	rec.P = v0.position * barycentric[0] + v1.position * barycentric[1] + v2.position * barycentric[2];
	rec.N = v0.normal * barycentric[0] +   v1.normal * barycentric[1] +   v2.normal * barycentric[2];
	rec.uv =v0.uv * barycentric[0] +       v1.uv * barycentric[1] +       v2.uv * barycentric[2];
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
		barycentric[0] = length0 / areaOfParellogram;
		barycentric[1] = length1 / areaOfParellogram;
		barycentric[2] = 1 - barycentric[0] - barycentric[1];

		// Record hit data!!!
		rec.t = t;
		rec.P = P;
		rec.N = N;
		rec.uv = barycentric[0] * v2.uv + barycentric[1] * v0.uv + barycentric[2] * v1.uv;
		rec.mat_ptr = mat_ptr;

		++rec.rayTriangleSuccess;

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

///////////////////////////////////////////////////////////////////////////////////////////////////
void Triangle::BoundingBox(AABB &box) const
{
	box.minBound = glm::vec3(fminf(fminf(v0.position[0], v1.position[0]), v2.position[0]),
							fminf(fminf(v0.position[1], v1.position[1]), v2.position[1]),
							fminf(fminf(v0.position[2], v1.position[2]), v2.position[2]));

	box.maxBound = glm::vec3(fmaxf(fmaxf(v0.position[0], v1.position[0]), v2.position[0]),
							fmaxf(fmaxf(v0.position[1], v1.position[1]), v2.position[1]),
							fmaxf(fmaxf(v0.position[2], v1.position[2]), v2.position[2]));
}

