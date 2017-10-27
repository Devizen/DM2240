#include "MasterEntityManager.h"
#include "EntityBase.h"
#include "../EntityManager.h"
#include "../SpatialPartition/SpatialPartitionManager.h"
#include "../SpatialPartition/PartitionInfo.h"

CMasterEntityManager* CMasterEntityManager::s_instance = 0;

CMasterEntityManager::CMasterEntityManager(void)
	: spatialPartitioning(true)
	, partitionNum(0)
{
}

CMasterEntityManager::~CMasterEntityManager(void)
{
}

CMasterEntityManager * CMasterEntityManager::GetInstance(void)
{
	if (s_instance == nullptr)
		s_instance = new CMasterEntityManager();

	return s_instance;
}

void CMasterEntityManager::Update(double _dt)
{
	if (spatialPartitioning)
		masterList[partitionNum]->Update(_dt);
	else
	{
		for (MasterList::iterator it = masterList.begin(); it != masterList.end(); ++it)
		{
			(*it)->Update(_dt);
		}
	}
}

void CMasterEntityManager::Render()
{
	if (spatialPartitioning)
		masterList[partitionNum]->Render();
	else
	{
		for (MasterList::iterator it = masterList.begin(); it != masterList.end(); ++it)
		{
			(*it)->Render();
		}
	}
}

void CMasterEntityManager::RenderUI()
{
	if (spatialPartitioning)
		masterList[partitionNum]->RenderUI();
	else
	{
		for (MasterList::iterator it = masterList.begin(); it != masterList.end(); ++it)
		{
			(*it)->RenderUI();
		}
	}
}

void CMasterEntityManager::RenderStatic(void)
{
	staticList->Render();
}

void CMasterEntityManager::RenderStaticUI(void)
{
	staticList->RenderUI();
}

void CMasterEntityManager::AddEntity(EntityBase * _newEntity)
{
	masterList[CSpatialPartitionManager::GetInstance()->GetEntityGrid(_newEntity->GetPosition())]->AddEntity(_newEntity);
}

void CMasterEntityManager::RemoveEntity(EntityBase* _newEntity, CPartitionInfo* _partitionInfo)
{
	masterList[_partitionInfo->GetPartition()]->RemoveEntity(_newEntity);
}

void CMasterEntityManager::AddStaticEntity(EntityBase * _newEntity)
{
	staticList->AddEntity(_newEntity);
}

void CMasterEntityManager::CheckPartition(EntityBase * _entity, CPartitionInfo* _partitionInfo)
{
	if (!CSpatialPartitionManager::GetInstance()->IsEntityInCorrectGrid(_partitionInfo))
	{
		this->RemoveEntity(_entity, _partitionInfo);
		this->AddEntity(_entity);
	}
}

void CMasterEntityManager::Init(size_t numOfEntityManager)
{
	for (size_t i = 0; i != numOfEntityManager; ++i)
	{
		EntityManager* manager = new EntityManager();
		masterList.push_back(manager);
	}
	staticList = new EntityManager();
}
