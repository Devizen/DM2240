#include "GenericEntity.h"
#include "MeshBuilder.h"
#include "EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
/*For categorising entity types.*/
#include "EntityTypes\EntityTypes.h"
#include "MasterEntityManager\MasterEntityManager.h"
#include "LevelOfDetail\LevelOfDetail.h"
#include "QuadTree\QuadTreeManager.h"
#include "Enemy\EnemyManager.h"
GenericEntity::GenericEntity()
{
}
GenericEntity::GenericEntity(Mesh* _modelMesh)
//: modelMesh(_modelMesh)
//, timer(0.f)
//, transformOffset(0.f)
//, transformChange(false)
//, sceneGraph(nullptr)
////, isParent(false)
//, parentNode(nullptr)
//, rootNode(nullptr)
//, name("")
{
	InitLoD(_modelMesh->name, _modelMesh->name, _modelMesh->name);
	this->modelMesh = _modelMesh;
	timer = (0.f);
	transformOffset = (0.f);
	transformChange = (false);
	sceneGraph = (nullptr);
	isParent = (false);
	parentNode = (nullptr);
	rootNode = (nullptr);
	name = ("");
}

GenericEntity::~GenericEntity()
{
}

void GenericEntity::Update(double _dt)
{
	/*Translate as a whole based on individual parts.*/
	position += rootNode->GetEntity()->GetDirection() * _dt;
	/*Translate based on own offset.*/
	position += direction * _dt;
}

void GenericEntity::UpdateChildren(double _dt)
{
	for (int i = 0; i < sceneGraph->GetRoot()->GetNumOfChild(); ++i) {
		sceneGraph->GetRoot()->GetEntity(i)->Update();
		sceneGraph->GetRoot()->GetEntity(i)->GetEntity()->Update(_dt);
	}
}

void GenericEntity::Render()
{
	if (LoD.GetCurrMesh() == nullptr)
	{
		//Render the big invisible AABB
		if (CEnemyManager::GetInstance()->GetRenderAABB())
		{
			std::vector<Vector3> allVertices;
			Vector3 min = collider->GetMinAABB();
			Vector3 max = collider->GetMaxAABB();

			/*Right Plane.*/
			allVertices.push_back(Vector3(max.x, min.y, min.z));
			allVertices.push_back(Vector3(max.x, min.y, max.z));
			allVertices.push_back(Vector3(max.x, max.y, max.z));
			allVertices.push_back(Vector3(max.x, max.y, min.z));
			/*Left Plane.*/
			allVertices.push_back(Vector3(min.x, min.y, min.z));
			allVertices.push_back(Vector3(min.x, min.y, max.z));
			allVertices.push_back(Vector3(min.x, max.y, max.z));
			allVertices.push_back(Vector3(min.x, max.y, min.z));
			/*Bottom Plane.*/
			allVertices.push_back(Vector3(min.x, min.y, min.z));
			allVertices.push_back(Vector3(min.x, min.y, max.z));
			allVertices.push_back(Vector3(max.x, min.y, max.z));
			allVertices.push_back(Vector3(max.x, min.y, min.z));
			/*Top Plane.*/
			allVertices.push_back(Vector3(min.x, max.y, min.z));
			allVertices.push_back(Vector3(min.x, max.y, max.z));
			allVertices.push_back(Vector3(max.x, max.y, max.z));
			allVertices.push_back(Vector3(max.x, max.y, min.z));
			//modelStack.PushMatrix();
			RenderHelper::DrawLine(allVertices, Color(1, 0, 0), 4);
			//modelStack.PopMatrix();
		}
		return;
	}

	Vector3 parentPosition(0.f, 0.f, 0.f);
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();

	modelStack.PushMatrix();
	modelStack.Translate(position.x,  position.y, position.z);
	///*Offsetting OBJ to center.*/
	//modelStack.PushMatrix();
	///*Only rotate the modelStack when there is axis selected.*/
	if (rotateAxis.LengthSquared() != 0.f) {
		if (rotateAxis.y != 0) {
			modelStack.PushMatrix();
			modelStack.Rotate(rotateAngle, 0.f, 1.f, 0.f);
			if (rotateAxis.x != 0) {
				modelStack.PushMatrix();
				modelStack.Translate(0.f, 10.f, 0.f);
				modelStack.Rotate(offset.x, 1.f, 0.f, 0.f);
				modelStack.Translate(0.f, -10.f, 0.f);
			}
		}
		if (rotateAxis.z != 0) {
			modelStack.PushMatrix();
			modelStack.Rotate(offset.z, 0.f, 0.f, 1.f);
		}
	}
	//	//modelStack.Translate(offset.x, offset.y, offset.z);
	//	//modelStack.Translate(-offset.x, -offset.y, -offset.z);
	//}
	//modelStack.PopMatrix();

	modelStack.Scale(scale.x, scale.y, scale.z);
	if (GetIsActive() == true) {
		if (GetLevel() != CLevelOfDetail::NONE) {
			//Ren
		}
	}

	if (LoD.GetIsActive())
		RenderHelper::RenderMesh(LoD.GetCurrMesh());
	else
		RenderHelper::RenderMesh(LoD.GetMesh(CLevelOfDetail::LEVEL::HIGH));


	if (rotateAxis.x != 0) {
		modelStack.PopMatrix();
	}
	if (rotateAxis.y != 0) {
		modelStack.PopMatrix();
	}
	if (rotateAxis.z != 0) {
		modelStack.PopMatrix();
	}

	modelStack.PopMatrix();

	if (CEnemyManager::GetInstance()->GetRenderAABB()) {
		std::vector<Vector3> allVertices;
		Vector3 min = collider->GetMinAABB();
		Vector3 max = collider->GetMaxAABB();

		/*Right Plane.*/
		allVertices.push_back(Vector3(max.x, min.y, min.z));
		allVertices.push_back(Vector3(max.x, min.y, max.z));
		allVertices.push_back(Vector3(max.x, max.y, max.z));
		allVertices.push_back(Vector3(max.x, max.y, min.z));
		/*Left Plane.*/
		allVertices.push_back(Vector3(min.x, min.y, min.z));
		allVertices.push_back(Vector3(min.x, min.y, max.z));
		allVertices.push_back(Vector3(min.x, max.y, max.z));
		allVertices.push_back(Vector3(min.x, max.y, min.z));
		/*Bottom Plane.*/
		allVertices.push_back(Vector3(min.x, min.y, min.z));
		allVertices.push_back(Vector3(min.x, min.y, max.z));
		allVertices.push_back(Vector3(max.x, min.y, max.z));
		allVertices.push_back(Vector3(max.x, min.y, min.z));
		/*Top Plane.*/
		allVertices.push_back(Vector3(min.x, max.y, min.z));
		allVertices.push_back(Vector3(min.x, max.y, max.z));
		allVertices.push_back(Vector3(max.x, max.y, max.z));
		allVertices.push_back(Vector3(max.x, max.y, min.z));
		modelStack.PushMatrix();
		RenderHelper::DrawLine(allVertices, Color(1, 0, 0), 4);
		modelStack.PopMatrix();
	}
}

void GenericEntity::RenderChildren()
{
	rootNode->GetEntity()->Render();
	for (int i = 0; i < sceneGraph->GetRoot()->GetNumOfChild(); ++i) {
		//sceneGraph->GetRoot()->GetEntity(i)->Render();
		sceneGraph->GetRoot()->GetEntity(i)->GetEntity()->Render();
	}
}

void GenericEntity::RenderAllNodes(CSceneNode* _node)
{
	if (_node == nullptr)
		return;

	_node->Render();
	if (_node->GetNumOfChild() > 0) {
		for (vector<CSceneNode*>::iterator it = _node->GetChildren().begin(); it != _node->GetChildren().end(); ++it) {
			RenderAllNodes(*it);
		}
	}
}

void GenericEntity::UpdateAllNodes(CSceneNode * _node, double _dt)
{
	if (_node == nullptr)
		return;

	_node->GetEntity()->Update(_dt);
	if (_node->GetNumOfChild() > 0) {
		for (vector<CSceneNode*>::iterator it = _node->GetChildren().begin(); it != _node->GetChildren().end(); ++it) {
			UpdateAllNodes((*it), _dt);
		}
	}
}

CSceneGraph * GenericEntity::GetSceneGraph()
{
	if (sceneGraph == nullptr)
		sceneGraph = new CSceneGraph();
	return sceneGraph;
}

// Set the maxAABB and minAABB
//void GenericEntity::SetAABB(Vector3 maxAABB, Vector3 minAABB)
//{
//	this->maxAABB = maxAABB;
//	this->minAABB = minAABB;
//}

GenericEntity* Create::Entity(	const std::string& _meshName, 
								const Vector3& _position,
								const Vector3& _scale)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh == nullptr)
		return nullptr;

	GenericEntity* result = new GenericEntity(modelMesh);
	result->SetPosition(_position);
	result->SetScale(_scale);
	result->SetCollider(false);
	//if (_meshName == "TERRAIN")
	//	CMasterEntityManager::GetInstance()->AddStaticEntity(result);
	//else
	//	CMasterEntityManager::GetInstance()->AddEntity(result);
	EntityManager::GetInstance()->AddEntity(result);
	return result;
}

GenericEntity * Create::Asset(const std::string & _meshName, const Vector3 & const _position, const Vector3 & _scale, const Vector3& _maxAABB, const bool& _parent)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh == nullptr)
		return nullptr;

	GenericEntity* result = new GenericEntity(modelMesh);
	result->SetName(_meshName);
	result->SetPosition(_position);
	result->SetScale(_scale);
	result->SetCollider(false);
	/*Min AABB followed by Max AABB.*/
	//result->SetAABB(Vector3(-_maxAABB.x * 0.5f, -_maxAABB.x * 0.5f, -_maxAABB.x * 0.5f),
	//	Vector3(_maxAABB.x * 0.5f, _maxAABB.x * 0.5f, _maxAABB.x * 0.5f));
	if (_parent) {
		QuadTreeManager::GetInstance()->InsertEntity(result);

		//EntityManager::GetInstance()->AddEntity(result);
		//result->GetSceneGraph()->AddNode(result);
		result->SetIsParent(true);
	}
	result->isStatic = true;
	return result;
}

#include "Manager\CollisionManager.h"
#include "Projectile\Projectile.h"
#include "PlayerInfo\ScoreManager.h"
//#include "Enemy\Monk\Monk.h"
void GenericEntity::CollisionResponse(EntityBase* other)
{
	//std::cout << "BOOM : " << other << std::endl;

	if (name == "MONK_HEAD" && dynamic_cast<CProjectile*>(other))
	{
		this->isStatic = false;
		if (rootNode)
		{
			rootNode->GetEntity()->isStatic = false;
		}
	}

	if (name == "MONK_BODY")
	{
		//std::cout << "BOM" << std::endl;
	}
	//if (this->sceneGraph)
	//{
	//	if (this->GetRootNode())
	//	{
	//		std::vector<CSceneNode*> children = this->GetRootNode()->GetChildren();
	//		for (auto child : children)
	//		{
	//			
	//			CollisionManager::GetInstance()->CheckCollision(other->collider, child->GetEntity()->collider, 1.0 / 60.0);
	//		}
	//	}
	//}

	if (isDestroyable == true)
	{
		if (name == "MONK_HEAD" && dynamic_cast<GenericEntity*>(other))
		{
			CScoreManager::GetInstance()->AddScore(-5.0);
		}
		isDone = true;
	}
	if (isStatic == false)
	{
		if (name == "MONK_HEAD" && dynamic_cast<GenericEntity*>(other))
		{
			CScoreManager::GetInstance()->AddScore(-5.0);
		}
		isDone = true;
	}
}