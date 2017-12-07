#include "EntityBase.h"
/*Categorise entities.*/
#include "EntityTypes\EntityTypes.h"
#include "Manager\CollisionManager.h"
#include "LevelOfDetail\LevelOfDetail.h"

EntityBase::EntityBase() 
	: position(0.0f, 0.0f, 0.0f)
	, rotateAngle(0.f)
	, rotateAxis(0.f, 0.f, 0.f)
	, scale(1.0f, 1.0f, 1.0f)
	, isDone(false)
	, m_bCollider(false)
	, entityType(ECEntityTypes::OTHERS)
	, collider(nullptr)
	//, levelOfDetail(ECLevelOfDetail::LOW)
{
}

EntityBase::~EntityBase()
{
	if (this->collider)
	{
		CollisionManager::GetInstance()->RemoveCollider(this->collider);
		delete this->collider;
	}
}

void EntityBase::Update(double _dt)
{
}

void EntityBase::Render()
{
}

void EntityBase::RenderUI()
{
}

void EntityBase::RenderTerrain(void)
{
}

bool EntityBase::IsDone()
{
	return isDone;
}

void EntityBase::SetIsDone(bool _value)
{
	isDone = _value;
}

// Check if this entity has a collider class parent
bool EntityBase::HasCollider(void) const
{
	return m_bCollider;
}

// Set the flag to indicate if this entity has a collider class parent
void EntityBase::SetCollider(const bool _value)
{
	m_bCollider = _value;
}
