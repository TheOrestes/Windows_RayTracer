
#include "AABB.h"
#include "Triangle.h"

//#define MOLLER_TRUMBORE

///////////////////////////////////////////////////////////////////////////////////////////////////
Triangle::Triangle()
{
	m_pTranform = new Transform();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Triangle::Triangle(const VertexPNT & _v0, const VertexPNT & _v1, const VertexPNT & _v2, Transform* _pTransform, Material * ptr_mat)
{
	m_pTranform = _pTransform;

	// Transform vertex positions using transformation matrix!
	v0.position = m_pTranform->matWorld * glm::vec4(_v0.position, 1);
	v1.position = m_pTranform->matWorld * glm::vec4(_v1.position, 1);
	v2.position = m_pTranform->matWorld * glm::vec4(_v2.position, 1);

	// Transform normals using Inverse Transpose of transformation matrix!
	v0.normal = m_pTranform->matInvTransposeWorld * glm::vec4(_v0.normal, 0);
	v1.normal = m_pTranform->matInvTransposeWorld * glm::vec4(_v1.normal, 0);
	v2.normal = m_pTranform->matInvTransposeWorld * glm::vec4(_v2.normal, 0);

	// Keep UVs as is...!
	v0.uv = _v0.uv;
	v1.uv = _v1.uv;
	v2.uv = _v2.uv;

	mat_ptr = ptr_mat;

	// calculate centroid...
	centroid = v0.position + v1.position + v2.position;
	centroid /= 3.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
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
	glm::vec3 transN = m_pTranform->matInvTransposeWorld * glm::vec4(N, 0);

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

