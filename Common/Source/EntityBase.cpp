#include "EntityBase.h"
/*Categorise entities.*/
#include "EntityTypes\EntityTypes.h"
#include "Manager\CollisionManager.h"
#include "LevelOfDetail\LevelOfDetail.h"
#include "Collider\Collider.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "Enemy\EnemyManager.h"

EntityBase::EntityBase()
	: position(0.0f, 0.0f, 0.0f)
	, direction(0.f, 0.f, 0.f)
	, offset(0.f, 0.f, 0.f)
	, rotateAngle(0.f)
	, rotateAxis(0.f, 0.f, 0.f)
	, scale(1.0f, 1.0f, 1.0f)
	, isDone(false)
	, m_bCollider(false)
	, entityType(ECEntityTypes::OTHERS)
	, collider(nullptr)
	, constMinAABB(0.f, 0.f, 0.f)
	, constMaxAABB(0.f, 0.f, 0.f)
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
	position += direction * _dt;

	collider->SetAABB(Vector3(constMaxAABB.x + position.x,
		constMaxAABB.y + position.y,
		constMaxAABB.z + position.z),

		Vector3(constMinAABB.x + position.x,
			constMinAABB.y + position.y,
			constMinAABB.z + position.z));
}

void EntityBase::Render()
{
	if (CEnemyManager::GetInstance()->GetRenderAABB()) {
		MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
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
		RenderHelper::DrawLine(allVertices, Color(0, 0, 1), 4);
		modelStack.PopMatrix();
	}
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
