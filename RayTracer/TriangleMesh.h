#pragma once

#include <vector>
#include <string>
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"
#include "assimp\scene.h"
#include "TriangleMeshInfo.h"
#include "Triangle.h"

class Material;
class AABB;
class BVHTree;

class TriangleMesh : public Hitable
{
public:
	TriangleMesh();
	~TriangleMesh();
	TriangleMesh(const MeshInfo& _meshInfo);

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
	virtual void BoundingBox(AABB& box) const;

	inline uint64_t GetTriangleCount() { return m_iTriangleCount; }

private:

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Triangle*> m_vecTriangles;

	AABB*					m_ptrAABB;
	Material*				m_ptrMaterial;
	Transform*				m_ptrTransform;
	MeshInfo*				m_ptrMeshInfo;

	BVHTree*				m_ptrBVH;

	uint64_t				m_iTriangleCount;
};
