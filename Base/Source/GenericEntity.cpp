#include "GenericEntity.h"
#include "MeshBuilder.h"
#include "EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
/*For categorising entity types.*/
#include "EntityTypes\EntityTypes.h"
#include "MasterEntityManager\MasterEntityManager.h"
GenericEntity::GenericEntity(Mesh* _modelMesh)
	: modelMesh(_modelMesh)
	, entityType(ECEntityTypes::OTHERS)
{
}

GenericEntity::~GenericEntity()
{
}

void GenericEntity::Update(double _dt)
{
	// Does nothing here, can inherit & override or create your own version of this class :D
}

void GenericEntity::Render()
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	switch (entityType)
	{
	case ECEntityTypes::OTHERS:
		modelStack.PushMatrix();
		modelStack.Translate(position.x, position.y, position.z);
		/*Only rotate the modelStack when there is axis selected.*/
		if (rotateAxis.LengthSquared() != 0.f)
			modelStack.Rotate(rotateAngle, rotateAxis.x, rotateAxis.y, rotateAxis.z);
		modelStack.Scale(scale.x, scale.y, scale.z);
		RenderHelper::RenderMeshWithLight(modelMesh);
		modelStack.PopMatrix();
		break;
	case ECEntityTypes::TERRAIN:
		MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
		modelStack.PushMatrix();
		modelStack.Translate(position.x, position.y, position.z);
		/*Only rotate the modelStack when there is axis selected.*/
		if (rotateAxis.LengthSquared() != 0.f)
			modelStack.Rotate(rotateAngle, rotateAxis.x, rotateAxis.y, rotateAxis.z);
		modelStack.Scale(scale.x, scale.y, scale.z);
		RenderHelper::RenderMeshWithLight(modelMesh);
		modelStack.PopMatrix();
		break;
	}
}

// Set the maxAABB and minAABB
void GenericEntity::SetAABB(Vector3 maxAABB, Vector3 minAABB)
{
	this->maxAABB = maxAABB;
	this->minAABB = minAABB;
}

void GenericEntity::SetEntityType(ECEntityTypes _entityType)
{
	entityType = _entityType;
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
	if (_meshName == "TERRAIN")
		CMasterEntityManager::GetInstance()->AddStaticEntity(result);
	else
		CMasterEntityManager::GetInstance()->AddEntity(result);
	return result;
}
