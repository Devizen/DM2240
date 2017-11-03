#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <list>
#include <vector>
#include "Vector3.h"

class EntityBase;

class EntityManager
{
	static EntityManager* s_instance;
public:
	static EntityManager* GetInstance(void);

	void Update(double _dt);
	void Render();
	void RenderUI();

	void AddEntity(EntityBase* _newEntity);
	bool RemoveEntity(EntityBase* _existingEntity);

	void UpdateCollision();
private:
	EntityManager();
	virtual ~EntityManager();

	// Check for overlap
	bool CheckOverlap(Vector3 thisMinAABB, Vector3 thisMaxAABB, Vector3 thatMinAABB, Vector3 thatMaxAABB);
	// Check if this entity's bounding sphere collided with that entity's bounding sphere 
	bool CheckSphereCollision(EntityBase *ThisEntity, EntityBase *ThatEntity);
	// Check if this entity collided with another entity, but both must have collider
	bool CheckAABBCollision(EntityBase *ThisEntity, EntityBase *ThatEntity);
	// Check if any Collider is colliding with another Collider
	bool CheckForCollision(void);

	/*Entity list to store entity that is in partition.*/
	typedef std::vector<EntityBase*> EntityList;
	EntityList entityList;

	void CollisionResponse(EntityBase* left, EntityBase* right);
};

#endif // ENTITY_MANAGER_H