#include "GenericEntity.h"
#include "MeshBuilder.h"
#include "EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
/*For categorising entity types.*/
#include "EntityTypes\EntityTypes.h"
#include "MasterEntityManager\MasterEntityManager.h"
#include "LevelOfDetail\LevelOfDetail.h"
GenericEntity::GenericEntity(Mesh* _modelMesh)
	: modelMesh(_modelMesh)
	, timer(0.f)
	, translateDirection(false)
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
}

void GenericEntity::Render()
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
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

GenericEntity * Create::Asset(const std::string & _meshName, const Vector3 & const _position, const Vector3 & _scale)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh == nullptr)
		return nullptr;

	GenericEntity* result = new GenericEntity(modelMesh);
	result->SetPosition(_position);
	result->SetScale(_scale);
	result->SetCollider(false);
	//EntityManager::GetInstance()->AddEntity(result);
	return result;
}
