#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include "../SingletonTemplate.h"

#include <set>
#include <map>
#include <list>
#include <unordered_map>
#include "../Vector3.h"

class CCollider;
class EntityBase;
class CollisionManager : public Singleton<CollisionManager>
{
	friend Singleton;
	//index of partition, set of colliders inside that partition
	
	std::map<int, std::set<CCollider*>> colliders;

	typedef std::unordered_map<CCollider*, std::list<unsigned int>*> CMasterMap;
	CMasterMap colliderMap;
public:
	void Update(double dt);
	void AddCollider(CCollider* collidable, int partition);
	void RemoveCollider(CCollider* collidable, int partition);
	void AddCollider(CCollider* collidable, std::list<unsigned int>* partition);
	void RemoveCollider(CCollider* collidable);
	void CheckCollision(CCollider* left, CCollider* right, double dt);

	std::vector<std::pair<Vector3, Vector3>> posColliderChecks;

	void RenderGrid();
protected:
	CollisionManager();
	virtual ~CollisionManager();

	bool isCollideAABB_AABB(CCollider* origin, CCollider* target);
	bool isCollideAABB_SPHERE(CCollider* origin, CCollider* Sphere);
	bool isCollideLine_AABB(CCollider* line, CCollider* AABB, double dt);

	//Line Collision
	bool GetIntersection(const float fDst1, const float fDst2, Vector3 P1, Vector3 P2, Vector3 &Hit);
	bool InBox(Vector3 Hit, Vector3 B1, Vector3 B2, const int Axis);
};

#endif // !COLLISION_MANAGER_H