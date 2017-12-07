#ifndef QUADTREEMANAGER_H
#define QUADTREEMANAGER_H

#include "../EntityManager.h"
#include "QuadTree.h"
#include <list>
#include <vector>
#include <deque>
#include <algorithm>

class QuadTreeManager
{
	QuadTreeManager();
	~QuadTreeManager();

	static QuadTreeManager* instance;

	QuadTree* root;
	const unsigned minSplitSize;

	std::deque<EntityBase*> entityList;

	void CheckTreeNode(QuadTree* node);
	std::list<EntityBase*> GetEntityList();
	std::vector<Vector3> GetGridVertices(QuadTree* node);
public:
	static QuadTreeManager* GetInstance() { return (instance ? instance : instance = new QuadTreeManager()); }
	void Update(double dt);
	void PostUpdate(double dt);
	void RenderGrid();
	void RenderGrid(QuadTree* node);

	int PutEntitiesInGrid(QuadTree* node, std::list<EntityBase*>& entityList);
	void InsertEntity(EntityBase* entity) { entityList.push_back(entity); };
	void RemoveEntity(EntityBase* entity) { entityList.erase(std::remove(entityList.begin(), entityList.end(), entity), entityList.end()); }

	bool wadawd = true;
};

#endif // !QUADTREEMANAGER_H
