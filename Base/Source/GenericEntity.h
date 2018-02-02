#ifndef GENERIC_ENTITY_H
#define GENERIC_ENTITY_H

#include "EntityBase.h"
#include <string>
#include "Collider/Collider.h"
#include "LevelOfDetail\LevelOfDetail.h"
#include "SceneGraph\SceneGraph.h"
#include "Enemy\AnimationPattern.h"
class Mesh;
/*For categorising entity.*/
enum class ECEntityTypes;
class GenericEntity : public EntityBase, public CLevelOfDetail, public SAnimationPattern
{
public:
	GenericEntity();
	GenericEntity(Mesh* _modelMesh);
	virtual ~GenericEntity();
	virtual void Init(void);
	virtual void Update(double _dt);
	void UpdateChildren(double _dt);
	virtual void Render();
	void RenderChildren();
	/*Pass in the SceneGraph of the entity (parent/root).*/
	void RenderAllNodes(CSceneNode* _node);
	/*Pass in the SceneGraph of the entity (parent/root).*/
	void UpdateAllNodes(CSceneNode* _node, double _dt);

	/*Set Min AABB and Max AABB.*/
	//void SetAABB(Vector3 _minAABB, Vector3 _maxAABB) { minAABB = _minAABB; maxAABB = _maxAABB; }
	//Vector3 GetMinAABB() { return minAABB; }
	//Vector3 GetMaxAABB() { return maxAABB; }

	void SetIsParent(bool _isParent) { isParent = _isParent; }

	virtual void CollisionResponse(EntityBase* other);

	/*Set SceneGraph for multiple objects tied together.*/
	void SetSceneGraph(CSceneGraph* _sceneGraph) { sceneGraph = _sceneGraph; }
	/*Get SceneGraph Node Count.*/
	size_t GetSceneGraphSize() { return sceneGraph->GetNumOfNode(); }
	/*Get SceneGraph.*/
	CSceneGraph* GetSceneGraph();

	bool GetIsParent() { return isParent; }

	/*Set Root Scene Node.*/
	void SetRootNode(CSceneNode* _rootNode) { rootNode = _rootNode; }
	/*Get Root Scene Node for traversal of update and render.*/
	CSceneNode* GetRootNode(void) { return rootNode; }
	/*Set Parent Scene Node.*/
	void SetParentNode(CSceneNode* _parentNode) { parentNode = _parentNode; }


	/*Add timer for animation.*/
	void AddTimer(float _timer) { timer += _timer; }
	/*Set timer.*/
	void SetTimer(float _timer) { timer = _timer; }
	/*Get timer.*/
	float GetTimer(void) { return timer; }
	/*Reset timer.*/
	void ResetTimer(void) { timer = 0.f; }

	/*Get transformChange boolean for switching between different transformation.*/
	bool GetTransformChange(void) { return transformChange; }
	/*Set transformChange.*/
	void SetTransformChange(bool _transformChange) { transformChange = _transformChange; }
	/*Toggle transformChange, true will be false and false will become true.*/
	void ToggleTransformChange(void) { transformChange = transformChange ? false : true; }

	/*Add transformOffset for animation.*/
	void AddTransformOffset(float _transformOffset) { transformOffset += _transformOffset; }
	/*Set transformOffset.*/
	void SetTransformOffset(float _transformOffset) { transformOffset = _transformOffset; }
	/*Get transformOffset.*/
	float GetTransformOffset(void) { return transformOffset; }
	/*Reset transformOffset.*/
	void ResetTransformOffset(void) { transformOffset = 0.f; }

	/*Set object name same as Mesh.*/
	void SetName(std::string _name) { name = _name; }
	/*Get object name.*/
	std::string GetName(void) { return name; }


protected:

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