
#include "LameBVH.h"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////////////////////
BVHTree::BVHTree()
{
	m_pRootNode = nullptr;
	m_uiNumNodes = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BVHTree::~BVHTree()
{
	if (m_pRootNode)
	{
		delete m_pRootNode;
		m_pRootNode = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BVHTree::hit(const Ray & ray, float tMin, float tMax, HitRecord & rec) const
{
	if (!m_pRootNode)
		return false;

	float closestSoFar = tMax;
	if (m_pRootNode->bbox.hit(ray, tMin, tMax, rec))
	{
		bool hit = Hit(m_pRootNode, ray, tMin, tMax, rec);
		return hit;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BVHTree::BoundingBox(AABB & box) const
{
	box = m_pRootNode->bbox;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BVHTree::Hit(BVHNode * node, const Ray & ray, float & tMin, float & tMax, HitRecord & rec) const
{
	bool isIntersection = false;

	// We are not at a leaf node so check the left and right node of the current node
	if (!node->isLeaf)
	{
		float tL0 = 0.0001f;
		float tL1 = FLT_MAX;

		float tR0 = 0.0001f;
		float tR1 = FLT_MAX;

		BVHNode *firstNode = 0;
		BVHNode *secondNode = 0;
		BVHNode *leftNode = node->leftNode;

		if (leftNode)
		{
			bool intersectedL = leftNode->bbox.hit(ray, tL0, tL1, rec);
			if (intersectedL && tL0 <= tMax)
			{
				firstNode = leftNode;
			}
		}

		BVHNode *rightNode = node->rightNode;
		if (rightNode)
		{
			bool intersectedR = rightNode->bbox.hit(ray, tR0, tR1, rec);
			if (intersectedR && tR0 <= tMax)
			{
				secondNode = rightNode;
			}
		}

		if (firstNode)
		{
			float thit1 = tMax;
			bool isIntersect1 = Hit(firstNode, ray, tMin, thit1, rec);

			if (isIntersect1 && thit1 < tMax)
			{
				tMax = thit1;
				isIntersection = true;
			}
		}

		if (secondNode)
		{
			float thit2 = tMax;

			bool isIntersect2 = Hit(secondNode, ray, tMin, thit2, rec);

			if (isIntersect2 && thit2 < tMax)
			{
				tMax = thit2;
				isIntersection = true;
			}
		}

	}
	// Check intersection for all triangles contained in the leaf node
	else
	{
		uint64_t startIndex = node->startIndex;
		uint64_t noOfTriangles = node->numTriangles;

		for (uint64_t i = startIndex; i < startIndex + noOfTriangles; i++)
		{
			if (primsVector->at(i)->hit(ray, tMin, tMax, rec))
			{
				isIntersection = true;
				// Record the closest hit
				tMax = rec.t;
			}
		}
	}

	return isIntersection;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BVHTree::BuildBVHTree(std::vector<Triangle*> *listTris, int _leafSize)
{
	if (!m_pRootNode)
	{
		// 1. Create a Root Node
		m_pRootNode = new BVHNode();

		// 2. Create AABB for every object in the scene.
		std::vector<Triangle*>::iterator iter = listTris->begin();
		AABB worldBB;
		for (; iter != listTris->end() ; iter++)
		{
			AABB tempBB;
			(*iter)->BoundingBox(tempBB);
			worldBB.ExpandBoundingBox(tempBB);
		}

		// 3. Assign AABB box to Root node.
		m_pRootNode->bbox = worldBB;
		m_pRootNode->startIndex = 0;
	}

	int leftIndex = 0;
	int rightIndex = listTris->size();

	m_iLeafSize = _leafSize;

	primsVector = listTris;
	BuildRecursive(leftIndex, rightIndex, m_pRootNode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void BVHTree::BuildRecursive(int startIndex, int endIndex, BVHNode * node)
{
	if ((endIndex - startIndex) <= m_iLeafSize)
	{
		node->isLeaf = true;
		node->startIndex = startIndex;
		node->numTriangles = endIndex - startIndex;

		m_uiNumNodes++;
	}
	else
	{
		// 4. Find AABB's longest axis & sort each object along this direction
		int longestAxis = node->bbox.GetLongestAxis();
		//float axisMidPoint = 0.0f;
		glm::vec3 nodeAxis = node->bbox.minBound + node->bbox.maxBound;
		float axisMidPoint = nodeAxis[longestAxis] * 0.5f;

		switch (longestAxis)
		{
		case 0:
		{
			//axisMidPoint = (node->bbox.maxBound[0] - node->bbox.minBound[0]) * 0.5f;
			std::sort(primsVector->begin() + startIndex, primsVector->begin() + endIndex, CompareBB_X);
			break;
		}

		case 1:
		{
			//axisMidPoint = (node->bbox.maxBound[1] - node->bbox.minBound[1]) * 0.5f;
			std::sort(primsVector->begin() + startIndex, primsVector->begin() + endIndex, CompareBB_Y);
			break;
		}

		case 2:
		{
			//axisMidPoint = (node->bbox.maxBound[2] - node->bbox.minBound[2]) * 0.5f;
			std::sort(primsVector->begin() + startIndex, primsVector->begin() + endIndex, CompareBB_Z);
			break;
		}
		}

		// 5. Find split index according to midPoint on largest axis
		int splitIndex = startIndex;
		for (int i = startIndex; i < endIndex; i++)
		{
			glm::vec3 centroid = primsVector->at(i)->Centroid();
			if (centroid[longestAxis] > axisMidPoint)
			{
				splitIndex = i;
				break;
			}
		}

		// 6. Using split index, divide the scene into left & right side
		// 7. For each side, create an AABB containing their respective objects
		AABB leftBB;
		primsVector->at(startIndex)->BoundingBox(leftBB);
		for (int i = startIndex+1; i < splitIndex; i++)
		{
			AABB tempBB;
			primsVector->at(i)->BoundingBox(tempBB);
			leftBB.ExpandBoundingBox(tempBB);
		}

		AABB rightBB;
		primsVector->at(splitIndex)->BoundingBox(rightBB);
		for (int i = splitIndex + 1; i < endIndex; i++)
		{
			AABB tempBB;
			primsVector->at(i)->BoundingBox(tempBB);
			rightBB.ExpandBoundingBox(tempBB);
		}

		// 8. Create a left & right node in the binary tree & attach it's corresponding BB.
		BVHNode* leftNode = new BVHNode();
		BVHNode* rightNode = new BVHNode();

		leftNode->bbox = leftBB;
		rightNode->bbox = rightBB;

		node->leftNode = leftNode;
		node->rightNode = rightNode;

		BuildRecursive(startIndex, splitIndex, leftNode);
		BuildRecursive(splitIndex + 1, endIndex, rightNode);
	}
	

	//if ((rightIndex - leftIndex) <= 1)
	//{
	//	node->isLeaf = true;
	//	node->startIndex = leftIndex;
	//}
	//else
	//{
	//	int longestAxis = node->bbox.GetLongestAxis();
	//	glm::vec3 nodeAxis = (node->bbox.minBound + node->bbox.maxBound);
	//	float midPointOnAxis = nodeAxis[longestAxis] * 0.5f;
	//
	//	switch (longestAxis)
	//	{
	//	case 0:
	//	{
	//		std::sort(primsVector->begin() + leftIndex, primsVector->begin() + rightIndex, CompareBB_X);
	//		break;
	//	}
	//
	//	case 1:
	//	{
	//		std::sort(primsVector->begin() + leftIndex, primsVector->begin() + rightIndex, CompareBB_Y);
	//		break;
	//	}
	//
	//	case 2:
	//	{
	//		std::sort(primsVector->begin() + leftIndex, primsVector->begin() + rightIndex, CompareBB_Z);
	//		break;
	//	}
	//	}
	//
	//	
	//	int splitIndex = leftIndex;
	//	for (int i = leftIndex; i < rightIndex; i++)
	//	{
	//		glm::vec3 centroid = primsVector->at(i)->Centroid();
	//		
	//		if (centroid[longestAxis] > midPointOnAxis)
	//		{
	//			splitIndex = i;
	//			break;
	//		}
	//	}
	//
	//	BVHNode* leftNode = new BVHNode();
	//	BVHNode* rightNode = new BVHNode();
	//
	//	AABB leftBB;
	//	primsVector->at(leftIndex)->BoundingBox(leftBB);
	//	for (int i = leftIndex + 1; i < splitIndex; i++)
	//	{
	//		AABB tempBB;
	//		primsVector->at(i)->BoundingBox(tempBB);
	//		leftBB.ExpandBoundingBox(tempBB);
	//	}
	//
	//	AABB rightBB;
	//	primsVector->at(splitIndex)->BoundingBox(rightBB);
	//	for (int i = splitIndex + 1; i < rightIndex; i++)
	//	{
	//		AABB tempBB;
	//		primsVector->at(i)->BoundingBox(tempBB);
	//		rightBB.ExpandBoundingBox(tempBB);
	//	}
	//
	//	leftNode->bbox = leftBB;
	//	rightNode->bbox = rightBB;
	//
	//	node->leftNode = leftNode;
	//	node->rightNode = rightNode;
	//
	//	BuildRecursive(leftIndex, splitIndex, leftNode);
	//	BuildRecursive(splitIndex, rightIndex, rightNode);
	//}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BVHTree::CompareBB_X(const Triangle * first, const Triangle * second)
{
	return (first->Centroid()[0] < second->Centroid()[0]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BVHTree::CompareBB_Y(const Triangle * first, const Triangle * second)
{
	return (first->Centroid()[1] < second->Centroid()[1]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool BVHTree::CompareBB_Z(const Triangle * first, const Triangle * second)
{
	return (first->Centroid()[2] < second->Centroid()[2]);
}



