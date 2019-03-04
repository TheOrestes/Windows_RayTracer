#pragma once

#include <vector>
#include <string>
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"
#include "assimp\scene.h"

#include "Triangle.h"

class Material;
class AABB;

class TriangleMesh : public Hitable
{
public:
	TriangleMesh();
	~TriangleMesh() {}
	TriangleMesh(const std::string& path, Material* ptr_mat);

	virtual bool hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const;

	inline int GetTriangleCount() { return m_iTriangleCount; }

private:

	void LoadModel(const std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Triangle*> m_vecTriangles;
	AABB*				   m_ptrAABB;
	Material*			   m_ptrMaterial;

	int					   m_iTriangleCount;
};
