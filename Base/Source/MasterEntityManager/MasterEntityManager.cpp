#include "MasterEntityManager.h"
#include "EntityBase.h"
#include "../EntityManager.h"
#include "../SpatialPartition/SpatialPartitionManager.h"]
//#include "../SpatialPartition/PartitionInfo.h"
#include "../SpatialPartition/SpatialPartition.h"

CMasterEntityManager* CMasterEntityManager::s_instance = 0;

CMasterEntityManager::CMasterEntityManager(void)
	: spatialPartitioning(true)
	, partitionNum(0)
{
}

CMasterEntityManager::~CMasterEntityManager(void)
{
	//for (size_t i = 0; i != masterList.size(); ++i)
	//{
	//	delete masterList[i];
	//}
	//delete staticList;
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
	{
		masterList[partitionNum]->Update(_dt);

		//objects that need changing of partition
		for (auto listType : allEntities)
		{
			for (EntityList::iterator it = listType.begin(); it != listType.end(); ++it)
			{
				EntityBase* entity = *it;

				//Here will check if entity is inside correct place, if not it will reassign new partition
				CheckPartition(entity);
			}
		}
	}
	else
	{
		for (MasterList::iterator it = masterList.begin(); it != masterList.end(); ++it)
		{
			//Update the EntityManager
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

	//masterList[CSpatialPartitionManager::GetInstance()->GetEntityGrid(_newEntity->GetPosition())]->AddEntity(_newEntity);

	//int index = CSpatialPartitionManager::GetInstance()->GetEntityGrid(_newEntity->GetPosition());
	//masterList[index]->AddEntity(_newEntity);
	//CSpatialPartition* spatialPartition = dynamic_cast<CSpatialPartition*>(_newEntity);
	//if (spatialPartition == nullptr)
	//	return;
	//spatialPartition->SetPartition(index);
	//spatialPartition->SetMinBoundary(CSpatialPartitionManager::GetInstance()->GetPartition(index)->GetMinBoundary());
	//spatialPartition->SetMaxBoundary(CSpatialPartitionManager::GetInstance()->GetPartition(index)->GetMaxBoundary());
	////Put entity inside masterEntity List according to type
	//allEntities[(unsigned int)(_newEntity->GetEntityType())].push_back(_newEntity);
}

void CMasterEntityManager::RemoveEntity(EntityBase* _newEntity)
{
	//masterList[_partitionInfo->GetPartition()]->RemoveEntity(_newEntity);

	CSpatialPartition* spatialPartition = dynamic_cast<CSpatialPartition*>(_newEntity);
	if (spatialPartition == nullptr)
		return;

	for (auto it : spatialPartition->GetPartition())
		masterList[it]->RemoveEntity(_newEntity);
	spatialPartition->ClearPartition();

	//Check its type and remove from master entity
	allEntities[(unsigned int)(_newEntity->GetEntityType())].remove(_newEntity);
}

void CMasterEntityManager::AddStaticEntity(EntityBase * _newEntity)
{
	staticList->AddEntity(_newEntity);
}

void CMasterEntityManager::CheckPartition(EntityBase * _entity)
{
	//if (!CSpatialPartitionManager::GetInstance()->IsEntityInCorrectGrid(_partitionInfo))
	//{
	//	this->RemoveEntity(_entity, _partitionInfo);
	//	this->AddEntity(_entity);
	//}

	CSpatialPartition* spatialPartition = dynamic_cast<CSpatialPartition*>(_entity);
	if (spatialPartition == nullptr)
		return;


	if (!CSpatialPartitionManager::GetInstance()->IsEntityInCorrectGrid(_entity))
	{
		this->RemoveEntity(_entity);
		this->AddEntity(_entity);
	}
}

void CMasterEntityManager::Init(size_t numOfEntityManager)
{
	//for (size_t i = 0; i != numOfEntityManager; ++i)
	//{
	//	EntityManager* manager = new EntityManager();
	//	masterList.push_back(manager);
	//}
	//staticList = new EntityManager();
}
