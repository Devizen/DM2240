#include "TowerManager.h"
#include "MeshBuilder.h"
#include "Manager\CollisionManager.h"
#include "../SpatialPartition/SpatialPartitionManager.h"
#include "../EntityTypes/EntityTypes.h"
CTowerManager* CTowerManager::s_instance = 0;
CTowerManager::CTowerManager()
{
}

CTowerManager::~CTowerManager()
{
}

CTowerManager * CTowerManager::GetInstance(void)
{
	if (s_instance == nullptr)
		s_instance = new CTowerManager();
	return s_instance;
}

void CTowerManager::AddTower(GenericEntity * _tower)
{
	towerList.push_back(_tower);
}

void CTowerManager::RemoveTower(GenericEntity * _tower)
{
	_tower->SetIsDone(true);
}

std::vector<GenericEntity*>& CTowerManager::GetTowerList(void)
{
	return towerList;
}

GenericEntity* Create::Tower(const Vector3 & _position, const Vector3 & _scale)
{
	GenericEntity* tower = Create::Asset("TOWER", _position, _scale, Vector3(50.f, 150.f, 50.f), true);
	/*Create root for Scene Graph.*/
	CSceneNode* rootNode = Create::SceneNode(nullptr, nullptr, nullptr);
	rootNode->GetEntity()->SetPosition(_position);
	/*Create an empty Scene Graph which will be rendered and updated in QuadTreeManager.
	The entire Scene Graph will be traversed from the root.*/
	tower->SetSceneGraph(Create::SceneGraph(rootNode));
	Create::SceneNode(rootNode, rootNode, tower);

	CTowerManager::GetInstance()->AddTower(tower);
	tower->SetEntityType(ECEntityTypes::OBJECT);
	tower->collider = new CCollider(tower);
	tower->collider->SetMinAABB(Vector3(tower->GetPosition().x - 5.f, tower->GetPosition().y, tower->GetPosition().z - 5.f));
	tower->collider->SetMaxAABB(Vector3(tower->GetPosition().x + 5.f, tower->GetPosition().y + 50.f, tower->GetPosition().z + 5.f));
	tower->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(tower->GetPosition(), tower->GetScale()));
	CollisionManager::GetInstance()->AddCollider(tower->collider, tower->GetPartitionPtr());
	tower->InitLoD("TOWER", "TOWER", "TOWER");

	/*Set AABB.*/
	rootNode->GetEntity()->SetEntityType(ECEntityTypes::OBJECT);
	rootNode->GetEntity()->collider = new CCollider(rootNode->GetEntity());
	rootNode->GetEntity()->collider->SetMaxAABB(tower->collider->GetMaxAABB() - tower->GetPosition());
	rootNode->GetEntity()->collider->SetMinAABB(tower->collider->GetMinAABB() - tower->GetPosition());
	rootNode->GetEntity()->constMaxAABB = rootNode->GetEntity()->collider->GetMaxAABB();
	rootNode->GetEntity()->constMinAABB = rootNode->GetEntity()->collider->GetMinAABB();
	rootNode->GetEntity()->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(rootNode->GetEntity()->GetPosition(), rootNode->GetEntity()->GetScale()));
	CollisionManager::GetInstance()->AddCollider(rootNode->GetEntity()->collider, rootNode->GetEntity()->GetPartitionPtr());
	return tower;
}
