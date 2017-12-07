#include "QuadTreeManager.h"

#include "EntityBase.h"
#include "RenderHelper.h"
#include "Manager\CollisionManager.h"

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

	renderCout = false;
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
		if (renderCout)
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

void QuadTreeManager::CheckCollision(std::vector<std::pair<Vector3, Vector3>>& posOfChecks, double dt)
{
	if (!root)
		return;

	std::vector<std::pair<Vector3, Vector3>> ret(CheckCollision(root, dt));
	posOfChecks.assign(ret.begin(), ret.end());
}

std::vector<std::pair<Vector3, Vector3>> QuadTreeManager::CheckCollision(QuadTree * node, double dt)
{
	std::vector<std::pair<Vector3, Vector3>> posOfChecks;

	if (!node)
		return posOfChecks;

	//check leaf node only
	if (node->GetAllChildren().size()) {
		
		std::vector<std::pair<Vector3, Vector3>> temp (CheckCollision(node->topLeft, dt));
		posOfChecks.insert(posOfChecks.end(), temp.begin(), temp.end());
		std::vector<std::pair<Vector3, Vector3>> temp2(CheckCollision(node->topRight, dt));
		posOfChecks.insert(posOfChecks.end(), temp2.begin(), temp2.end());
		std::vector<std::pair<Vector3, Vector3>> temp3(CheckCollision(node->bottomLeft, dt));
		posOfChecks.insert(posOfChecks.end(), temp3.begin(), temp3.end());
		std::vector<std::pair<Vector3, Vector3>> temp4(CheckCollision(node->bottomRight, dt));
		posOfChecks.insert(posOfChecks.end(), temp4.begin(), temp4.end());

		return posOfChecks;
	}



	for (std::list<EntityBase*>::iterator it = node->entityList.begin(); it != node->entityList.end(); ++it) {

		std::list<EntityBase*>::iterator it2 = std::next(it, 1);
		for (; it2 != node->entityList.end(); ++it2)
		{
			CollisionManager::GetInstance()->CheckCollision((*it)->collider, (*it2)->collider, dt);
			posOfChecks.push_back(std::make_pair((*it)->GetPosition(), (*it2)->GetPosition()));
		}
	}

	return posOfChecks;
}

