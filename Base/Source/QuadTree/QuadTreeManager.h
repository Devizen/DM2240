#ifndef QUADTREEMANAGER_H
#define QUADTREEMANAGER_H

#include "../EntityManager.h"
#include "QuadTree.h"
#include <list>
#include <vector>
#include <deque>
#include <algorithm>
#include <utility>

class GroundEntity;
class GenericEntity;
class QuadTreeManager
{
	QuadTreeManager();
	~QuadTreeManager();

	static QuadTreeManager* instance;

	QuadTree* root;
	const unsigned minSplitSize;

	std::deque<EntityBase*> entityList;

	std::deque<EntityBase*> preUpdateRemovalList;

	void CheckTreeNode(QuadTree* node);
	std::list<EntityBase*> GetEntityList();
	std::vector<Vector3> GetGridVertices(QuadTree* node);
	//returns 0 if totally outside, returns 1 if one point inside, return 2 if all inside
	int IsAABBInGrid(Vector3 min, Vector3 max, QuadTree* node);


	////more optimsied
	//std::list<EntityBase*> GetNearbyEntities(GenericEntity* entity, QuadTree* root);

	void RenderObj(QuadTree* node);
	void UpdateLeafNode(QuadTree* node, double dt);
public:
	static QuadTreeManager* GetInstance() { return (instance ? instance : instance = new QuadTreeManager()); }
	void Update(double dt);
	void PostUpdate(double dt);
	void RenderGrid();
	void RenderObj();

	void Clear();

	int PutEntitiesInGrid(QuadTree* node, std::list<EntityBase*>& entityList);
	void InsertEntity(EntityBase* entity) { entityList.push_back(entity);
	//std::cout << entityList.size() << std::endl;
	};
	void RemoveEntity(EntityBase* entity) { entityList.erase(std::remove(entityList.begin(), entityList.end(), entity), entityList.end()); 
	//std::cout << entityList.size() << std::endl;
	}

	bool renderCout = true;

	bool toggle;
	void CheckCollision(std::vector<std::pair<Vector3, Vector3>>& posOfChecks, double dt);
	std::vector<std::pair<Vector3, Vector3>> CheckCollision(QuadTree* node, double dt);

	struct AOEQuad
	{
		double dt;
		Vector3 pos;
		Vector3 scale;
		friend bool operator==(const AOEQuad& left, const AOEQuad& other) {
			return (left.pos == other.pos) && (left.scale == other.scale);
		}
	};
	//This func finds the parent grid that encaps the whole entity.
	std::list<EntityBase*> GetNearbyEntities(EntityBase* entity, QuadTree* leafNode, bool addToRender = false);
	QuadTree* GetRoot() { return root; }


	std::vector<AOEQuad> renderList;
	std::vector<AOEQuad> hitScanList;

	void ClearEntities() {
		for (auto e : entityList) {
			delete e;
			e = nullptr;
		}
		entityList.clear();
	};

	std::deque<EntityBase*>& GetAllEntities() { return entityList; };

	GroundEntity* ground;
};

#endif // !QUADTREEMANAGER_H
