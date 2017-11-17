#include "EntityManager.h"
#include "EntityBase.h"
#include "Collider/Collider.h"

/*Spatial Partition Manager for checking which entityList to push entity to.*/
#include "SpatialPartition\SpatialPartitionManager.h"

#include "EntityTypes\EntityTypes.h"

#include "PlayerInfo\PlayerInfo.h"

#include "LevelOfDetail\LevelOfDetail.h"

#include "SceneGraph\SceneGraph.h"

#include <iostream>
using namespace std;

EntityManager* EntityManager::s_instance = 0;

EntityManager * EntityManager::GetInstance(void)
{
	if (s_instance == nullptr)
		s_instance = new EntityManager();
	return s_instance;
}

// Update all entities
void EntityManager::Update(double _dt)
{
	// Update all entities
	for (EntityList::iterator it = entityList.begin(); it != entityList.end(); ++it)
	{
		CSpatialPartitionManager* spManager = CSpatialPartitionManager::GetInstance();
		CPlayerInfo* player = spManager->GetPlayer();
		float gridSize = spManager->GetGridLength() * spManager->GetGridLength();
		if ((*it)->GetEntityType() != ECEntityTypes::STATIC)
		{
			if (((*it)->GetPosition() - player->GetPos()).LengthSquared() >= gridSize * 3.f)
				(*it)->SetLevelOfDetail(ECLevelOfDetail::LOW);
			else if (((*it)->GetPosition() - player->GetPos()).LengthSquared() < gridSize * 3.f && 
				((*it)->GetPosition() - player->GetPos()).LengthSquared() >= gridSize * 2.f)
				(*it)->SetLevelOfDetail(ECLevelOfDetail::NORMAL);
			else
				(*it)->SetLevelOfDetail(ECLevelOfDetail::HIGH);

			(*it)->Update(_dt);
		}
	}

	CSceneGraph::GetInstance()->Update();
	for (EntityList::iterator it = entityList.begin(); it != entityList.end(); )
	{
		
		if (CSpatialPartitionManager::GetInstance()->UpdateGridInfo((*it)->GetPosition()) == nullptr 
			|| (*it)->IsDone())
		{
			delete (*it);
			it = entityList.erase(it);
		}
		else
			++it;
		
	}

	// Clean up entities that are done
	//EntityList::iterator it = entityList.begin();
	//EntityList::iterator end = entityList.end();
	//while (it != end)
	//{
	//	if ((*it)->IsDone())
	//	{
	//		// Delete if done
	//		delete *it;
	//		it = entityList.erase(it);
	//	}
	//	else
	//	{
	//		// Move on otherwise
	//		++it;
	//	}
	//}
}

// Render all entities
void EntityManager::Render()
{
	// Render all entities
	for (EntityList::iterator it = entityList.begin(); it != entityList.end(); ++it)
	{
		(*it)->Render();
	}
	CSceneGraph::GetInstance()->Render();
}

// Render the UI entities
void EntityManager::RenderUI()
{
	// Render all entities UI
	for (EntityList::iterator it = entityList.begin(); it != entityList.end(); ++it)
	{
		(*it)->RenderUI();
	}
}

// Add an entity to this EntityManager
void EntityManager::AddEntity(EntityBase* _newEntity)
{
	entityList.push_back(_newEntity);
}

// Remove an entity from this EntityManager
bool EntityManager::RemoveEntity(EntityBase* _existingEntity)
{
	// Find the entity's iterator
	EntityList::iterator findIter = std::find(entityList.begin(), entityList.end(), _existingEntity);

	// Delete the entity if found
	if (findIter != entityList.end())
	{
		//delete *findIter;
		findIter = entityList.erase(findIter);
		return true;	
	}
	// Return false if not found
	return false;
}

void EntityManager::UpdateCollision()
{
	for (EntityList::iterator it = entityList.begin(); it != entityList.end(); ++it)
	{
		if (!(*it)->HasCollider())
			continue;
		for (EntityList::iterator it2 = it + 1; it2 != entityList.end(); ++it2)
		{
			if (!(*it2)->HasCollider())
				continue;

			//Check if same Type
			if ((*it)->GetEntityType() == (*it2)->GetEntityType())
				continue;

			
		}
	}
}

// Constructor
EntityManager::EntityManager()
{
}

// Destructor
EntityManager::~EntityManager()
{
}

// Check for overlap
bool EntityManager::CheckOverlap(Vector3 thisMinAABB, Vector3 thisMaxAABB, Vector3 thatMinAABB, Vector3 thatMaxAABB)
{	
	return false;
}

// Check if this entity's bounding sphere collided with that entity's bounding sphere 
bool EntityManager::CheckSphereCollision(EntityBase *ThisEntity, EntityBase *ThatEntity)
{

	return false;
}

// Check if this entity collided with another entity, but both must have collider
bool EntityManager::CheckAABBCollision(EntityBase *ThisEntity, EntityBase *ThatEntity)
{

	return false;
}

// Check if any Collider is colliding with another Collider
bool EntityManager::CheckForCollision(void)
{
	return false;
}

void EntityManager::CollisionResponse(EntityBase * left, EntityBase * right)
{
	//Add all response here

	//CPlayerInfo* player = dynamic_cast<CPlayerInfo*>(left);


}
