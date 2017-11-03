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
	if (levelOfDetail == ECLevelOfDetail::HIGH)
		RenderHelper::RenderMeshWithLight(modelMesh);
	else if (levelOfDetail == ECLevelOfDetail::NORMAL)
		RenderHelper::RenderMeshWithLight(MeshBuilder::GetInstance()->GetMesh("sphere"));
	else if (levelOfDetail == ECLevelOfDetail::LOW)
		RenderHelper::RenderMeshWithLight(MeshBuilder::GetInstance()->GetMesh("cube"));
	modelStack.PopMatrix();
}

// Set the maxAABB and minAABB
void GenericEntity::SetAABB(Vector3 maxAABB, Vector3 minAABB)
{
	this->maxAABB = maxAABB;
	this->minAABB = minAABB;
}

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
