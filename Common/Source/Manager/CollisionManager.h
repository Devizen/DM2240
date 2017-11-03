#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include "../SingletonTemplate.h"

#include <set>
#include <map>

class CCollider;
class EntityBase;
class CollisionManager : public Singleton<CollisionManager>
{
	friend Singleton;
	//index of partition, set of colliders inside that partition
	
	std::map<int, std::set<CCollider*>> colliders;

public:
	void update(double dt);
	void add_collider(CCollider* collidable, int partition);
	void remove_collider(CCollider* collidable);
	//void add_ground(Collidable* ground);
	//Collidable* get_ground();
	void CollisionResponse(EntityBase* left, EntityBase* right);
protected:
	CollisionManager();
	virtual ~CollisionManager();
};

#endif // !COLLISION_MANAGER_H