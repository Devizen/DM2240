#include "QuadTreeManager.h"

#include "EntityBase.h"
#include "RenderHelper.h"

QuadTreeManager* QuadTreeManager::instance = nullptr;

QuadTreeManager::QuadTreeManager() : root(nullptr), entityList(entityList), minSplitSize(2U)
{
}

QuadTreeManager::~QuadTreeManager()
{
}

void QuadTreeManager::Update(double dt)
{
	//delete tree
	if (root)
	{
		//delete
		root->DeleteChildren();
		delete root;
		root = nullptr;
	}

	//recreate tree
	if (entityList.size() > minSplitSize)
	{
		std::list<EntityBase*> eList = this->GetEntityList();
		root = new QuadTree(Vector3(-200, 0, -200), Vector3(-200, 0, -200), Vector3(200, 0, 200), 0, 4, eList);

		//check split
		this->CheckTreeNode(root);

		root->Update(dt);
	}

	wadawd = false;
}

void QuadTreeManager::PostUpdate(double dt)
{
	for (int i = 0; i < entityList.size();)
	{
		if (entityList[i]->IsDone())
			this->RemoveEntity(entityList[i]);
		else
			++i;
	}
}

void QuadTreeManager::CheckTreeNode(QuadTree * node)
{
	if (!node)
		return;

	if (node->entityList.size() >= minSplitSize)
	{
		if (wadawd)
			std::cout << "Pos : " << node->position << "  Min : " << node->minBoundary <<
			"  Max : " << node->maxBoundary << std::endl;
		node->Split();

		this->CheckTreeNode(node->topLeft);
		this->CheckTreeNode(node->topRight);
		this->CheckTreeNode(node->bottomLeft);
		this->CheckTreeNode(node->bottomRight);
	}
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
	result.push_back(Vector3(min.x,0, min.z));
	result.push_back(Vector3(min.x,0, max.z));
	result.push_back(Vector3(max.x,0, max.z));
	result.push_back(Vector3(max.x,0, min.z));

	std::vector<QuadTree*> childArray (node->GetAllChildren());
	if (childArray.empty())
		return result;
	for (int i = 0; i < childArray.size(); ++i)
	{
		std::vector<Vector3> childVertices(GetGridVertices(childArray[i]));
		result.insert(result.end(), childVertices.begin(), childVertices.end());
	}
	
	return result;
}


void QuadTreeManager::RenderGrid()
{
	std::vector<Vector3> allVertices(this->GetGridVertices(root));

	RenderHelper::DrawLine(allVertices, Color(1, 0, 0), 4);

//	RenderGrid(root);
}

void QuadTreeManager::RenderGrid(QuadTree * node)
{
	if (!node)
		return;

	node->Render();
	RenderGrid(node->topLeft);
	RenderGrid(node->topRight);
	RenderGrid(node->bottomLeft);
	RenderGrid(node->bottomRight);
}

int QuadTreeManager::PutEntitiesInGrid(QuadTree * node, std::list<EntityBase*>& entityList)
{
	//here we extract the entites that fall inside the node and put them inside
	Vector3 pos = node->GetPosition();
	Vector3 min = node->GetMinBoundary();
	Vector3 max = node->GetMaxBoundary();

	int pushed = 0;
	for (std::list<EntityBase*>::iterator it = entityList.begin(); it != entityList.end(); ++it)
	{
		EntityBase* e = (*it);

		if (e->GetPosition().x >= min.x && e->GetPosition().y >= min.y && e->GetPosition().z >= min.z
			&& e->GetPosition().x < max.x && e->GetPosition().y < max.y && e->GetPosition().z < max.z) {
			++pushed;
			node->entityList.push_back(e);
		}
	}

	return pushed;
}

