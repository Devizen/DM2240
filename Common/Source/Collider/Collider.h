#pragma once
#include "../Vector3.h"
class EntityBase;
class CCollider
{
	EntityBase* owner;
public:
	CCollider(EntityBase* owner = nullptr);
	virtual ~CCollider();

	// Set the maxAABB and minAABB
	virtual void SetAABB(Vector3 maxAABB, Vector3 minAABB);
	// Set the maxAABB
	virtual void SetMaxAABB(Vector3 maxAABB);
	// Get the maxAABB
	virtual Vector3 GetMaxAABB(void);
	// Set the minAABB
	virtual void SetMinAABB(Vector3 minAABB);
	// Get the minAABB
	virtual Vector3 GetMinAABB(void);

	EntityBase* GetOwner() { return owner; }

	Vector3 corners[8];

	/*Set the center of AABB position for calculation of min and max.*/
	virtual void SetAABBPosition(Vector3 _AABBPosition) { AABBPosition = _AABBPosition; }
	virtual void SetAABBPosition(float _x, float _y, float _z) { AABBPosition.x = _x; AABBPosition.y = _y; AABBPosition.z = _z; }
	virtual Vector3 GetAABBPosition(void) { return AABBPosition; }
protected:
	/*The center position of AABB.*/
	Vector3 AABBPosition;
	// Vector3 variables to store coordinates of the AABB bounding box of an object
	Vector3 maxAABB;
	Vector3 minAABB;

};
