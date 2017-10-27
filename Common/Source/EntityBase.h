#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include "Vector3.h"
enum class ECEntityTypes;

class EntityBase
{
public:
	EntityBase();
	virtual ~EntityBase();

	virtual void Update(double _dt);
	virtual void Render();
	virtual void RenderUI();
	/*Terrain*/
	virtual void RenderTerrain(void);

	inline void SetPosition(const Vector3& _value){ position = _value; };
	inline Vector3 GetPosition(){ return position; };

	inline void SetRotateAngle(const float& _value) { rotateAngle = _value; };
	inline float GetRotateAngle() { return rotateAngle; };

	inline void SetRotateAxis(const Vector3& _value) { rotateAxis = _value; };
	inline Vector3 GetRotateAxis() { return rotateAxis; };

	inline void SetScale(const Vector3& _value) { scale = _value; };
	inline Vector3 GetScale() { return scale; };

	bool IsDone();
	void SetIsDone(const bool _value);

	// Check if this entity has a collider class parent
	virtual bool HasCollider(void) const;
	// Set the flag to indicate if this entity has a collider class parent
	virtual void SetCollider(const bool _value);

	//Get Type, Using for Collision check
	ECEntityTypes GetEntityType() { return entityType; }
	void SetEntityType(ECEntityTypes type) { this->entityType = type; }
protected:
	Vector3 position;
	float rotateAngle;
	Vector3 rotateAxis;
	Vector3 scale;
	ECEntityTypes entityType;

	bool isDone;
	bool m_bCollider;
};

#endif // ENTITY_BASE_H