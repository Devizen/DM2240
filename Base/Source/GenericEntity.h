#ifndef GENERIC_ENTITY_H
#define GENERIC_ENTITY_H

#include "EntityBase.h"
#include <string>
#include "Collider/Collider.h"
#include "LevelOfDetail\LevelOfDetail.h"
#include "SceneGraph\SceneGraph.h"
class Mesh;
/*For categorising entity.*/
enum class ECEntityTypes;
class GenericEntity : public EntityBase, public CLevelOfDetail
{
public:
	GenericEntity(Mesh* _modelMesh);
	virtual ~GenericEntity();

	virtual void Update(double _dt);
	void UpdateChildren(double _dt);
	virtual void Render();
	void RenderChildren();

	/*Set Min AABB and Max AABB.*/
	void SetAABB(Vector3 _minAABB, Vector3 _maxAABB) { minAABB = _minAABB; maxAABB = _maxAABB; }

	void SetIsParent(bool _isParent) { isParent = _isParent; }

	virtual void CollisionResponse(EntityBase* other) {
		std::cout << "BOOM : " << other << std::endl;
		isDone = true;
	}


	/*Get SceneGraph Node Count.*/
	size_t GetSceneGraphSize() { return sceneGraph->GetNumOfNode(); }
	/*Get SceneGraph.*/
	CSceneGraph* GetSceneGraph();
	bool GetIsParent() { return isParent; }

	CSceneNode* parentNode;
private:
	bool isParent;
	Vector3 minAABB;
	Vector3 maxAABB;
	Mesh* modelMesh;

	/*Demo-ing Spatial Partitioning.*/
	float timer;
	bool translateDirection;
	CSceneGraph* sceneGraph;
};

namespace Create
{
	GenericEntity* Entity(const std::string& _meshName,
		const Vector3& _position,
		const Vector3& _scale = Vector3(1.0f, 1.0f, 1.0f));
	GenericEntity* Asset(const std::string& _meshName,
		const Vector3& const _position,
		const Vector3& _scale = Vector3(1.0f, 1.0f, 1.0f),
		const Vector3& _maxAABB = Vector3(1.f, 1.f, 1.f),
		const bool& _parent = false);
};

#endif // GENERIC_ENTITY_H