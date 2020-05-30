///////////////////////////////////////////////////////////////////////////////////////////////////
// Based on Implementation by : https://github.com/DarrenSweeney/Dazzer_Ray
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AABB.h"
#include "Triangle.h"

#include <vector>

class Triangle;

struct BVHNode
{
	BVHNode()
	{
		leftNode = nullptr;
		rightNode = nullptr;
		isLeaf = false;
		startIndex = 0;
		numTriangles = 0;
	}

	~BVHNode()
	{
		if (!isLeaf)
		{
			if (leftNode)
			{
				delete leftNode;
				leftNode = nullptr;
			}

			if (rightNode)
			{
				delete rightNode;
				rightNode = nullptr;
			}
		}
	}

	BVHNode* leftNode;
	BVHNode* rightNode;
	AABB	 bbox;
	bool	 isLeaf;
	uint64_t startIndex;
	uint64_t numTriangles;
};

class BVHTree : public Hitable
{
public:
	BVHTree();
	~BVHTree();

	virtual bool		hit(const Ray& ray, float tMin, float tMax, HitRecord& rec) const;
	virtual void		BoundingBox(AABB& box) const;

	virtual float		PDF(const glm::vec3& origin, const glm::vec3& direction) const { return 1.0f; };
	virtual glm::vec3	Sample(const glm::vec3& origin) const { return glm::vec3(1.0f); };

	bool				Hit(BVHNode *node, const Ray &ray, float &tMin, float &tMax, HitRecord &rec) const;
	void				BuildBVHTree(std::vector<Triangle*> *listTris, int _leafSize);
	void				BuildRecursive(int startIndex, int endIndex, BVHNode* node);

	static bool			CompareBB_X(const Triangle * first, const Triangle * second);
	static bool			CompareBB_Y(const Triangle * first, const Triangle * second);
	static bool			CompareBB_Z(const Triangle * first, const Triangle * second);

private:
	BVHNode*			m_pRootNode;
	uint64_t			m_uiNumNodes;
	int					m_iLeafSize;
	std::vector<Triangle*> *primsVector;
};
