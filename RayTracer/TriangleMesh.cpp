
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

	m_iTriangleCount = 0;

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
	std::vector<VertexPNT> vecVertices;
	vecVertices.reserve(mesh->mNumVertices);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		VertexPNT vertex;

		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->mTextureCoords[0])
		{
			//vertex.uv = glm::clamp(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y), 0.0f, 1.0f);
			vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		
		vecVertices.push_back(vertex);
		m_ptrAABB->UpdateBB(vertex.position);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		int numIndices = face.mNumIndices;

		unsigned int index0 = face.mIndices[0];
		unsigned int index1 = face.mIndices[1]; 
		unsigned int index2 = face.mIndices[2];

		VertexPNT vert0 = vecVertices.at(index0);
		VertexPNT vert1 = vecVertices.at(index1);
		VertexPNT vert2 = vecVertices.at(index2);

		Triangle* tri = new Triangle(vert0, vert1, vert2, m_ptrMaterial);

		m_vecTriangles.push_back(tri);
	}

	// Hold count for triangles...
	m_iTriangleCount = m_vecTriangles.size();
}

/////////////////////////////////////////////////////////////////////////////////////////
bool TriangleMesh::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	bool isIntersection = false;
	float closestSoFar = tmax;

	//if (m_ptrAABB->hit(r, tmin, tmax, rec))
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
