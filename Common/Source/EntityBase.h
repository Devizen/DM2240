#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include "Vector3.h"
#include "../../Base/Source/SpatialPartition/SpatialPartition.h"
#include "../../Base/Source/SceneGraph/SceneNode.h"
#include "../../Base/Source/LevelOfDetail/LevelOfDetail.h"
//enum class ECLevelOfDetail;
enum class ECEntityTypes;
class CSceneGraph;
class CCollider;
class EntityBase : public CSpatialPartition, public CSceneNode
{
public:
	EntityBase();
	virtual ~EntityBase();

	virtual void Init(void);
	virtual void Update(double _dt);
	virtual void Render();
	virtual void RenderUI();
	/*Terrain*/
	virtual void RenderTerrain(void);

	/*Adders.*/
	void AddPosition(const Vector3& _position) { position += _position; };
	inline void SetPosition(const Vector3& _value) { position = _value; };
	inline void SetPosition(const float& _x, const float& _y, const float& _z) { position.x = _x; position.y = _y; position.z = _z; };
	inline Vector3 GetPosition() { return position; };

	inline void SetDirection(const Vector3& _value) { direction = _value; };
	inline void SetDirection(const float& _x, const float& _y, const float& _z) { direction.x = _x; direction.y = _y; direction.z = _z; };
	inline Vector3 GetDirection() { return direction; };

	inline void SetOffset(const Vector3& _value) { offset = _value; };
	inline void SetOffset(const float& _x, const float& _y, const float& _z) { offset.x = _x; offset.y = _y; offset.z = _z; };
	inline Vector3 GetOffset() { return offset; };

	inline void SetRotateAngle(const float& _value) { rotateAngle = _value; };
	inline float GetRotateAngle() { return rotateAngle; };

	inline void SetRotateAxis(const Vector3& _value) { rotateAxis = _value; };
	inline void SetRotateAxis(const float& _x, const float& _y, const float& _z) { rotateAxis.x = _x; rotateAxis.y = _y; rotateAxis.z = _z; };
	inline Vector3 GetRotateAxis() { return rotateAxis; };

	inline void SetScale(const Vector3& _value) { scale = _value; };
	inline Vector3 GetScale() { return scale; };

	bool IsDone();
	void SetIsDone(const bool _value);

	// Check if this entity has a collider class parent
	virtual bool HasCollider(void) const;
	// Set the flag to indicate if this entity has a collider class parent
	virtual void SetCollider(const bool _value);
	// Response
	virtual void CollisionResponse(EntityBase* other);

	/*Entity types.*/
	/*Set entity type.*/
	virtual void SetEntityType(ECEntityTypes _entityType) { entityType = _entityType; }
	//Get Type, Using for Collision check
	ECEntityTypes GetEntityType() { return entityType; }

	/*Level of Detail*/
	/*Set level of detail. To be used in conjunction with distance to player.*/
	// void SetLevelOfDetail(ECLevelOfDetail _levelOfDetail) { levelOfDetail = _levelOfDetail; }
	/*Get level of detail. To determine what type of object quality should be rendered.*/
	//ECLevelOfDetail GetLevelOfDetail(void) { return levelOfDetail; }

	//Collision
	CCollider* collider;

	bool InitLoD(const std::string& _meshNameHigh,
		const std::string& _meshNameMedium,
		const std::string& _meshNameLow) {
		return LoD.Init(_meshNameHigh, _meshNameMedium, _meshNameLow);
	}
	CLevelOfDetail& GetLoD() { return LoD; }
	Vector3 constMinAABB;
	Vector3 constMaxAABB;

	bool isParent=false;

	void UpdateChildren(double _dt);
	CSceneGraph* sceneGraph;

	bool isStatic = false;
	Vector3 rootMinAABB, rootMaxAABB;
	CSceneNode* rootNode;
	bool isDone;
	Vector3 scale;
	std::string name;
	bool isDestroyable = false;
protected:
	Vector3 position;
	/*For offsetting position for rotation when OBJ is not at center.*/
	Vector3 offset;
	/*For translating the entire Scene Graph.*/
	Vector3 direction;
	float rotateAngle;
	Vector3 rotateAxis;

	ECEntityTypes entityType;
	/*Initialise levelOfDetail to low quality.*/
	//CLevelOfDetail levelOfDetail;

	CLevelOfDetail LoD;


	bool m_bCollider;



	//Vector3 minAABB;
	//Vector3 maxAABB;
	Mesh* modelMesh;

	/*Demo-ing Spatial Partitioning.*/
	float timer;
	bool transformChange;
	float transformOffset;

	/*For following the transformation.*/

	CSceneNode* parentNode;


};

#endif // ENTITY_BASE_H