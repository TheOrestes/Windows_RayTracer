
#include "glm/glm.hpp"

#include "LameBVH.h"
#include "TriangleMesh.h"
#include "Material.h"
#include "Lambertian.h"
#include "Emissive.h"
#include "Metal.h"
#include "Transparent.h"
#include "Texture.h"
#include "AABB.h"
#include <Windows.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
TriangleMesh::TriangleMesh()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
TriangleMesh::~TriangleMesh()
{
	m_vecTriangles.clear();

	if (m_ptrMaterial)
	{
		delete m_ptrMaterial;
		m_ptrMaterial = nullptr;
	}

	if (m_ptrTransform)
	{
		delete m_ptrTransform;
		m_ptrTransform = nullptr;
	}

	if (m_ptrAABB)
	{
		delete m_ptrAABB;
		m_ptrAABB = nullptr;
	}

	if (m_ptrBVH)
	{
		delete m_ptrBVH;
		m_ptrBVH = nullptr;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
TriangleMesh::TriangleMesh(const MeshInfo& _meshInfo)
{
	m_vecTriangles.clear();
	m_ptrMaterial = nullptr;

	m_ptrMeshInfo = const_cast<MeshInfo*>(&_meshInfo);

	m_ptrTransform = new Transform(_meshInfo.position, _meshInfo.rotationAxis, _meshInfo.rotationAngle, _meshInfo.scale);
	m_ptrAABB = new AABB();

	LoadModel(_meshInfo.filePath);

	m_iTriangleCount = m_vecTriangles.size();

	m_ptrBVH = new BVHTree();
	m_ptrBVH->BuildBVHTree(&m_vecTriangles, _meshInfo.leafSize);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////////
void TriangleMesh::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<VertexPNT> vecVertices;
	vecVertices.reserve(mesh->mNumVertices);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		VertexPNT vertex;

		vertex.position = glm::vec3(mesh->mVertices[i][0], mesh->mVertices[i][1], mesh->mVertices[i][2]);
		vertex.normal = glm::vec3(mesh->mNormals[i][0], mesh->mNormals[i][1], mesh->mNormals[i][2]);

		if (mesh->mTextureCoords[0])
		{
			//vertex.uv = glm::clamp(glm::vec2(mesh->mTextureCoords[0][i][0], mesh->mTextureCoords[0][i][1]), 0.0f, 1.0f);
			vertex.uv = glm::vec2(mesh->mTextureCoords[0][i][0], mesh->mTextureCoords[0][i][1]);
		}
		
		vecVertices.push_back(vertex);
	}

	// process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiString aiName;

		if (AI_SUCCESS == aiGetMaterialString(material, AI_MATKEY_NAME, &aiName))
		{
			std::string name = aiName.C_Str();

			if (name.find("lambert") != std::string::npos)
			{
				// Extract texture info if filepath or flat color?
				Texture* textureInfo = nullptr;
				
				// Look if material has texture info...
				aiString path;
				if (AI_SUCCESS == aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &path))
				{
					std::string filePath = std::string(path.C_Str());
					textureInfo = new ImageTexture("models/" + filePath);
					m_ptrMaterial = new Lambertian(textureInfo);
				}
				else
				{
					// Check if we have set albedo color explicitly or not, if not then use Maya's 
					// set color from the properties!
					glm::vec4 albedoCol = m_ptrMeshInfo->matInfo.albedoColor;
					if (glm::length(albedoCol) == 0)
					{
						aiColor4D diffuseColor;
						aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
						albedoCol = glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
					}

					// Is this Mesh a light source?
					if (m_ptrMeshInfo->isLightSource)
					{
						textureInfo = new ConstantTexture(albedoCol);
						m_ptrMaterial = new Emissive(textureInfo);
					}
					else
					{
						textureInfo = new ConstantTexture(albedoCol);
						m_ptrMaterial = new Lambertian(textureInfo);
					}
				}
			}
			else if (name.find("metal") != std::string::npos)
			{
				// Extract texture info if filepath or flat color?
				Texture* textureInfo = nullptr;
				float roughness = m_ptrMeshInfo->matInfo.roughness;

				// Look if material has texture info...
				aiString path;
				if (AI_SUCCESS == aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &path))
				{
					std::string filePath = std::string(path.C_Str());
					textureInfo = new ImageTexture("models/" + filePath);
					m_ptrMaterial = new Metal(textureInfo, roughness);
				}
				else
				{
					// Check if we have set albedo color explicitly or not, if not then use Maya's 
					// set color from the properties!
					glm::vec4 albedoCol = m_ptrMeshInfo->matInfo.albedoColor;
					if (glm::length(albedoCol) == 0)
					{
						aiColor4D diffuseColor;
						aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
						albedoCol = glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
					}

					textureInfo = new ConstantTexture(albedoCol);
					m_ptrMaterial = new Metal(textureInfo, roughness);
				}
			}
			else if (name.find("transparent") != std::string::npos)
			{
				// Extract texture info if filepath or flat color?
				Texture* textureInfo = nullptr;
				float r_i = m_ptrMeshInfo->matInfo.refrIndex;

				// Check if we have set albedo color explicitly or not, if not then use Maya's 
				// set color from the properties!
				glm::vec4 albedoCol = m_ptrMeshInfo->matInfo.albedoColor;
				if (glm::length(albedoCol) == 0)
				{
					aiColor4D diffuseColor;
					aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
					albedoCol = glm::vec4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
				}

				textureInfo = new ConstantTexture(albedoCol);
				m_ptrMaterial = new Transparent(textureInfo, r_i);
			}
			else
			{
				MessageBox(0, L"Unknown Material", L"Error", MB_OK);
				return;
			}
		}
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

		Triangle* tri = new Triangle(vert0, vert1, vert2, m_ptrTransform, m_ptrMaterial);

		m_vecTriangles.push_back(tri);
	}

	//AABB box;
	//for (unsigned int i = 0; i < m_vecTriangles.size(); i++)
	//{
	//	m_vecTriangles[i]->BoundingBox(box);
	//	m_ptrAABB->ExpandBoundingBox(box);
	//}

	// Hold count for triangles...
	m_iTriangleCount = m_vecTriangles.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool TriangleMesh::hit(const Ray& r, float tmin, float tmax, HitRecord& rec) const
{
	bool isIntersection = false;
	float closestSoFar = tmax;

	if (m_ptrBVH->hit(r, tmin, tmax, rec))
	{
		//rec.mat_ptr = m_ptrMaterial;
		//closestSoFar = rec.t;
		isIntersection = true;
	}

	//if (m_ptrAABB->hit(r, tmin, tmax, rec))
	//{
	//	for (int i = 0; i < m_vecTriangles.size(); i++)
	//	{
	//		if (m_vecTriangles[i]->hit(r, tmin, closestSoFar, rec))
	//		{
	//			rec.mat_ptr = m_ptrMaterial;
	//			isIntersection = true;
	//			closestSoFar = rec.t;
	//		}
	//	}
	//}
	
	return isIntersection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TriangleMesh::BoundingBox(AABB & box) const
{
	box = *m_ptrAABB;
}

