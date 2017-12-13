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
GenericEntity::GenericEntity(Mesh* _modelMesh)
	: modelMesh(_modelMesh)
	, timer(0.f)
	, translateDirection(false)
	, sceneGraph(nullptr)
	, isParent(false)
	, parentNode(nullptr)
	, rootNode(nullptr)
{
}

GenericEntity::~GenericEntity()
{
}

void GenericEntity::Update(double _dt)
{
	// Does nothing here, can inherit & override or create your own version of this class :D
	timer += _dt;

	if (timer > 1.f)
	{
		translateDirection = translateDirection ? false : true;
		timer = 0.f;
	}

	if (!translateDirection)
		position = Vector3(position.x, position.y + (_dt * 10.f), position.z);
	else
		position = Vector3(position.x, position.y - (_dt * 10.f), position.z);

	if (isParent) {
		static float offset = 0.f;
		offset += static_cast<float>(_dt);
		static bool translateLoop = false;
		EntityBase* parent = GetSceneGraph()->GetRoot()->GetEntity();
		if (offset > 2.f) {
			translateLoop = translateLoop ? false : true;
			offset = 0.f;
		}
		if (translateLoop)
			//parent->AddPosition(Vector3(static_cast<float>(_dt), 0.f, 0.f));
			parent->SetPosition(static_cast<float>(_dt), parent->GetPosition().y, parent->GetPosition().z);
		else
			//parent->AddPosition(Vector3(-static_cast<float>(_dt), 0.f, 0.f));
			parent->SetPosition(-static_cast<float>(_dt), parent->GetPosition().y, parent->GetPosition().z);
	}

	position += rootNode->GetEntity()->GetPosition();
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
	Vector3 parentPosition(0.f, 0.f, 0.f);
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	//if (isParent) {
		//parentPosition.Set(parentNode->GetEntity()->GetPosition().x,
		//	parentNode->GetEntity()->GetPosition().y,
		//	parentNode->GetEntity()->GetPosition().z);
		//position += parentPosition;
		//std::cout << position << std::endl
	//}
	modelStack.Translate(position.x, position.y, position.z);
	/*Only rotate the modelStack when there is axis selected.*/
	if (rotateAxis.LengthSquared() != 0.f)
		modelStack.Rotate(rotateAngle, rotateAxis.x, rotateAxis.y, rotateAxis.z);
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
	//if (levelOfDetail == CLevelOfDetail::HIGH)
	//RenderHelper::RenderMesh(modelMesh);
	//else if (levelOfDetail == CLevelOfDetail::MEDIUM)
	//	RenderHelper::RenderMesh(/*MeshBuilder::GetInstance()->GetMesh("BLUESPHERE")*/modelMesh);
	//else if (levelOfDetail == CLevelOfDetail::LOW)
	//	RenderHelper::RenderMesh(/*MeshBuilder::GetInstance()->GetMesh("REDSPHERE")*/modelMesh);
	modelStack.PopMatrix();


	//AABB
	//if (this->collider == nullptr)
	//	return;
	//Vector3 min = this->collider->GetMinAABB();
	//Vector3 max = this->collider->GetMaxAABB();
	//Color color(1, 0, 0);
	//RenderHelper::DrawLine(Vector3(min.x, min.y, min.z), Vector3(min.x, max.y, min.z), color);
	//RenderHelper::DrawLine(Vector3(min.x, min.y, max.z), Vector3(min.x, max.y, max.z), color);
	//RenderHelper::DrawLine(Vector3(max.x, min.y, max.z), Vector3(max.x, max.y, max.z), color);
	//RenderHelper::DrawLine(Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, min.z), color);
}

void GenericEntity::RenderChildren()
{
	for (int i = 0; i < sceneGraph->GetRoot()->GetNumOfChild(); ++i) {
		sceneGraph->GetRoot()->GetEntity(i)->Render();
		sceneGraph->GetRoot()->GetEntity(i)->GetEntity()->Render();
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
	result->SetPosition(_position);
	result->SetScale(_scale);
	result->SetCollider(false);
	/*Min AABB followed by Max AABB.*/
	result->SetAABB(Vector3(-_maxAABB.x * 0.5f, -_maxAABB.x * 0.5f, -_maxAABB.x * 0.5f),
		Vector3(_maxAABB.x * 0.5f, _maxAABB.x * 0.5f, _maxAABB.x * 0.5f));
	if (_parent) {
		QuadTreeManager::GetInstance()->InsertEntity(result);
		//EntityManager::GetInstance()->AddEntity(result);
		result->GetSceneGraph()->AddNode(result);
		result->SetIsParent(true);
	}
	return result;
}
