#include "CollisionManager.h"

#include "../Collider/Collider.h"
//#include "MyDebugger.h"

CollisionManager::CollisionManager()
{

}
CollisionManager::~CollisionManager() {

}

void CollisionManager::update(double dt)
{
	for (std::map<int, std::set<CCollider*>>::iterator partitionIter = colliders.begin(); partitionIter != colliders.end(); ++partitionIter)
	{
		for (std::set<CCollider*>::iterator first_iter = (partitionIter->second).begin();
			first_iter != (partitionIter->second).end(); ++first_iter)
		{

			std::set<CCollider*>::iterator second_iter = first_iter;
			for (std::advance(second_iter, 1); second_iter != (partitionIter->second).end(); ++second_iter)
			{
				//check collision
				//response is activated inside the function if true
				//(*first_iter)->check_collision(*second_iter);

				//MyDebugger::GetInstance()->watch_this_info("first col", &(*first_iter));
				//MyDebugger::GetInstance()->watch_this_info("second col", &(*second_iter));
			}
		}
	}

	

	//MyDebugger::GetInstance()->watch_this_info("COLLISION SIZE", colliders.size());
}

//void CollisionManager::add_collider(CCollider* collidable)
//{
//	this->colliders.insert(collidable);
//}
//
//void CollisionManager::remove_collider(Collider * collidable)
//{
//	this->colliders.erase(collidable);
//}
//
//void CollisionManager::add_ground(Collidable * ground)
//{
//	this->ground = ground;
//}
//
//Collidable * CollisionManager::get_ground()
//{
//	return ground;
//}
