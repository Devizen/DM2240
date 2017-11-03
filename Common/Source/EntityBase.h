#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include "Vector3.h"

enum class ECLevelOfDetail;
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

	/*Entity types.*/
	/*Set entity type.*/
	virtual void SetEntityType(ECEntityTypes _entityType) { entityType = _entityType; }
	//Get Type, Using for Collision check
	ECEntityTypes GetEntityType() { return entityType; }

	/*Level of Detail*/
	/*Set level of detail. To be used in conjunction with distance to player.*/
	virtual void SetLevelOfDetail(ECLevelOfDetail _levelOfDetail) { levelOfDetail = _levelOfDetail; }
	/*Get level of detail. To determine what type of object quality should be rendered.*/
	ECLevelOfDetail GetLevelOfDetail(void) { return levelOfDetail; }
protected:
	Vector3 position;
	float rotateAngle;
	Vector3 rotateAxis;
	Vector3 scale;
	ECEntityTypes entityType;
	/*Initialise levelOfDetail to low quality.*/
	ECLevelOfDetail levelOfDetail;

	bool isDone;
	bool m_bCollider;
};

#endif // ENTITY_BASE_H