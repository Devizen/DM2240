#ifndef QUADTREEMANAGER_H
#define QUADTREEMANAGER_H

#include "../EntityManager.h"
#include "QuadTree.h"
#include <list>
#include <vector>

class QuadTreeManager
{
	QuadTreeManager(EntityManager::EntityList& entityList);
	~QuadTreeManager();

	static QuadTreeManager* instance;

	QuadTree* root;
	EntityManager::EntityList &entityList;
	const unsigned minSplitSize;

	void CheckTreeNode(QuadTree* node);
	std::list<EntityBase*> GetEntityList();
	std::vector<Vector3> GetGridVertices(QuadTree* node);
public:
	static QuadTreeManager* GetInstance() { return (instance ? instance : instance = new QuadTreeManager(EntityManager::GetInstance()->entityList)); }
	void Update(double dt);
	void RenderGrid();
};

#endif // !QUADTREEMANAGER_H
