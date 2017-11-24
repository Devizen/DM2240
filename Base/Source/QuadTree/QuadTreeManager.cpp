#include "QuadTreeManager.h"

QuadTreeManager* QuadTreeManager::instance = nullptr;

QuadTreeManager::QuadTreeManager(EntityManager::EntityList& entityList) : root(nullptr), entityList(entityList), minSplitSize(2U)
{
}

QuadTreeManager::~QuadTreeManager()
{
}

void QuadTreeManager::Update(double dt)
{
	if (root)
	{
		//delete
		root->DeleteChildren();
		delete root;
		root = nullptr;

		//if wan more optimized, instead of delete all, check them
		//this is to make sure simple quadtree is up
		//"premature optimization is root of all evil"
		//just like premature eje... - ohy
	}

	//create bois
	if (entityList.size() > minSplitSize)
	{
		std::list<EntityBase*> eList = this->GetEntityList();
		root = new QuadTree(Vector3(), Vector3(-400, -400, 0), Vector3(400, 400, 0), 0, 4, eList);
		root->Update(dt);
	}
}

void QuadTreeManager::CheckTreeNode(QuadTree * node)
{
}

std::list<EntityBase*> QuadTreeManager::GetEntityList()
{
	std::list<EntityBase*> eList;
	for (int i = 0; i < this->entityList.size(); ++i)
	{
		//can edit who to give or who not to give
		eList.push_back(this->entityList[i]);
	}
	return eList;
}

std::vector<Vector3> QuadTreeManager::GetGridVertices(QuadTree * node)
{
	std::vector<Vector3> result;

	if (node == nullptr)
		return result;

	//push ur vertices back
	Vector3 min = node->GetMinBoundary();
	Vector3 max = node->GetMaxBoundary();
	result.push_back(Vector3(min.x, min.y));
	result.push_back(Vector3(min.x, max.y));
	result.push_back(Vector3(max.x, max.y));
	result.push_back(Vector3(max.x, min.y));

	std::vector<QuadTree*> childArray = node->GetAllChildren();
	if (childArray.empty())
		return result;
	for (int i = 0; i < childArray.size(); ++i)
	{
		std::vector<Vector3> childVertices = GetGridVertices(childArray[i]);
		result.insert(result.end(), childVertices.begin(), childVertices.end());
	}
	
	return result;
}

void QuadTreeManager::RenderGrid()
{
	std::vector<Vector3> allVertices = this->GetGridVertices(root);


}

