#pragma once

#include <vector>
#include <string>
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"
#include "assimp\scene.h"

#include "Triangle.h"

class Material;
class AABB;
class BVHTree;

class TriangleMesh : public Hitable
{
public:
	TriangleMesh();
	~TriangleMesh();
	TriangleMesh(const std::string& path, Material* ptr_mat, uint32_t _leafSize);
	TriangleMesh(const std::string & path, uint32_t _leafSize);

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;
	virtual void BoundingBox(AABB& box) const;

	inline uint64_t GetTriangleCount() { return m_iTriangleCount; }

private:

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Triangle*> m_vecTriangles;

	AABB*				   m_ptrAABB;
	Material*			   m_ptrMaterial;

	BVHTree*			   m_ptrBVH;

	uint32_t			   m_iLeafSize;
	uint64_t			   m_iTriangleCount;
};
