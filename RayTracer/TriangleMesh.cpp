
#include "glm\glm.hpp"
#include "TriangleMesh.h"
#include "AABB.h"
#include <Windows.h>

TriangleMesh::TriangleMesh()
{

}

TriangleMesh::TriangleMesh(const std::string& path, Material* ptr_mat)
{
	m_vecTriangles.clear();
	m_ptrMaterial = ptr_mat;

	m_ptrAABB = new AABB();

	LoadModel(path);
}

void TriangleMesh::LoadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		MessageBox(0, L"Assimp Error!", L"Error", MB_OK);
		return;
	}

	// process root node recursively!
	ProcessNode(scene->mRootNode, scene);
}

void TriangleMesh::ProcessNode(aiNode* node, const aiScene* scene)
{
	// node only contains indices to actual objects in the scene. But scene,
	// conatins all the data, node is just to keep things organized.
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene);
	}

	// Once we have processed all the meshes, we recursively process
	// each child node
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

void TriangleMesh::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<aiVector3D> vecVertices;
	vecVertices.reserve(mesh->mNumVertices);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vecVertices.push_back(mesh->mVertices[i]);
		m_ptrAABB->UpdateBB(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace* face = &(mesh->mFaces[i]);
		int numIndices = face->mNumIndices;

		unsigned int index0 = face->mIndices[0];
		unsigned int index1 = face->mIndices[1]; 
		unsigned int index2 = face->mIndices[2];

		glm::vec3 pos0(vecVertices.at(index0).x, vecVertices.at(index0).y, vecVertices.at(index0).z);
		glm::vec3 pos1(vecVertices.at(index1).x, vecVertices.at(index1).y, vecVertices.at(index1).z);
		glm::vec3 pos2(vecVertices.at(index2).x, vecVertices.at(index2).y, vecVertices.at(index2).z);

		Triangle* tri = new Triangle(pos0, pos1, pos2, m_ptrMaterial);

		m_vecTriangles.push_back(tri);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
bool TriangleMesh::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	bool isIntersection = false;
	float closestSoFar = tmax;

	if (m_ptrAABB->hit(r, tmin, tmax))
	{
		for (int i = 0; i < m_vecTriangles.size(); i++)
		{
			if (m_vecTriangles[i]->hit(r, tmin, closestSoFar, rec))
			{
				isIntersection = true;
				closestSoFar = rec.t;
			}
		}
	}
	
	return isIntersection;
}
