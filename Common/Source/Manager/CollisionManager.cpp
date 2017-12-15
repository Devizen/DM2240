#include "CollisionManager.h"

#include "../Collider/Collider.h"
//#include "MyDebugger.h"
#include "../Collider/LineCollider.h"
#include "../EntityBase.h"
#include "SpatialPartition\SpatialPartitionManager.h"
#include "QuadTree\QuadTreeManager.h"
#include "../RenderHelper.h"

CollisionManager::CollisionManager()
{

}
CollisionManager::~CollisionManager() {

}

void CollisionManager::Update(double dt)
{
	if (QuadTreeManager::GetInstance()->toggle)
	{
		QuadTreeManager::GetInstance()->CheckCollision(posColliderChecks, dt);
	}
	else if (CSpatialPartitionManager::GetInstance()->toggle)
	{
		//build up colliders
		for (CMasterMap::iterator it = colliderMap.begin(); it != colliderMap.end(); ++it) {
			for (unsigned int i : *(it->second))
				colliders[i].insert(it->first);
		}

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
					if ((*first_iter)->GetOwner()->GetEntityType() == (*second_iter)->GetOwner()->GetEntityType())
						continue;

					this->CheckCollision((*first_iter), (*second_iter), dt);

					posColliderChecks.push_back(std::make_pair((*first_iter)->GetOwner()->GetPosition(),
						(*second_iter)->GetOwner()->GetPosition()));
					//MyDebugger::GetInstance()->watch_this_info("first col", &(*first_iter));
					//MyDebugger::GetInstance()->watch_this_info("second col", &(*second_iter));
				}
			}
		}

		colliders.clear();
	}
	else
	{
		for (CMasterMap::iterator it = colliderMap.begin(); it != colliderMap.end(); ++it) {

			for (CMasterMap::iterator it2 = std::next(it, 1); it2 != colliderMap.end(); ++it2) {

				if ((*it).first->GetOwner()->GetEntityType() == (*it2).first->GetOwner()->GetEntityType())
					continue;

				this->CheckCollision((*it).first, (*it2).first, dt);

				posColliderChecks.push_back(std::make_pair((*it).first->GetOwner()->GetPosition(),
					(*it2).first->GetOwner()->GetPosition()));
			}
		}
	}
	

	//MyDebugger::GetInstance()->watch_this_info("COLLISION SIZE", colliders.size());
}

void CollisionManager::AddCollider(CCollider* collidable, int partition)
{
	this->colliders[partition].insert(collidable);
	
}

void CollisionManager::RemoveCollider(CCollider * collidable, int partition)
{
	this->colliders.at(partition).erase(collidable);
}

void CollisionManager::AddCollider(CCollider * collidable, std::list<unsigned>* partition)
{
	//for each (int i in partition) {
	//	this->colliders[i].insert(collidable);
	//}
	this->colliderMap.insert(std::make_pair(collidable, partition));
}

void CollisionManager::RemoveCollider(CCollider * collidable)
{
	//for each (int i in partition) {
	//	this->colliders[i].erase(collidable);
	//}
	this->colliderMap.erase(collidable);
}

void CollisionManager::CheckCollision(CCollider * left, CCollider * right, double dt)
{
	LineCollider* line = dynamic_cast<LineCollider*>(left);
	if (line)
	{
		LineCollider* line2 = dynamic_cast<LineCollider*>(right);
		if (line2)
		{
			//line to line
		}
		else
		{
			//line to AABB
			if (isCollideLine_AABB(line, right, dt)) {

				line->GetOwner()->CollisionResponse(right->GetOwner());
				right->GetOwner()->CollisionResponse(line->GetOwner());
			}
		}
	}
	else
	{
		LineCollider* line2 = dynamic_cast<LineCollider*>(right);
		if (line2)
		{
			//aabb to line
			if (isCollideLine_AABB(line2, left, dt)) {
				line2->GetOwner()->CollisionResponse(left->GetOwner());
				left->GetOwner()->CollisionResponse(line2->GetOwner());
			}
		}
		else
		{
			//aabb to aabb
			if (isCollideAABB_AABB(left, right))
			{
				left->GetOwner()->CollisionResponse(right->GetOwner());
				right->GetOwner()->CollisionResponse(left->GetOwner());
			}
		}
	}
}

bool CollisionManager::isCollideAABB_AABB(CCollider* origin, CCollider * target)
{
	return (origin->GetMaxAABB() >= target->GetMinAABB() &&
		origin->GetMinAABB() <= target->GetMaxAABB());
}

bool CollisionManager::isCollideAABB_SPHERE(CCollider* origin, CCollider * Sphere)
{

	//float x = Math::Max(origin.GetMinAABB().x, Math::Min(Sphere.mid->x, this->mid->x + this->max.x));
	//float y = Math::Max(this->mid->y + this->min.y, Math::Min(Sphere.mid->y, this->mid->y + this->max.y));
	//float z = Math::Max(this->mid->z + this->min.z, Math::Min(Sphere.mid->z, this->mid->z + this->max.z));

	//float distSq = (x - Sphere.mid->x) * (x - Sphere.mid->x) +
	//	(y - Sphere.mid->y) * (y - Sphere.mid->y) +
	//	(z - Sphere.mid->z) * (z - Sphere.mid->z);

	//return distSq <= (Sphere.radius * Sphere.radius);
	return false;
}

bool CollisionManager::isCollideLine_AABB(CCollider * line, CCollider * AABB, double dt)
{
	LineCollider* linecheck = dynamic_cast<LineCollider*>(line);
	if (!linecheck)
		return false;
	LineCollider& lineCollider = *linecheck;
	Vector3 line_start(lineCollider.GetPos());
	Vector3 line_end(lineCollider.GetPos() - lineCollider.GetDir() * lineCollider.GetSpeed() * static_cast<float>(dt));
	Vector3 minAABB(AABB->GetMinAABB());
	Vector3 maxAABB(AABB->GetMaxAABB());
	Vector3 Hit;

	if ((GetIntersection(line_start.x - minAABB.x, line_end.x - minAABB.x,
		line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 1))
		|| (GetIntersection(line_start.y - minAABB.y, line_end.y -
			minAABB.y, line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 2))
		|| (GetIntersection(line_start.z - minAABB.z, line_end.z -
			minAABB.z, line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 3))
		|| (GetIntersection(line_start.x - maxAABB.x, line_end.x -
			maxAABB.x, line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 1))
		|| (GetIntersection(line_start.y - maxAABB.y, line_end.y -
			maxAABB.y, line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 2))
		|| (GetIntersection(line_start.z - maxAABB.z, line_end.z -
			maxAABB.z, line_start, line_end, Hit) && InBox(Hit, minAABB, maxAABB, 3)))
		return true;
	return false;

}

bool CollisionManager::GetIntersection(const float fDst1, const float fDst2, Vector3 P1, Vector3 P2, Vector3 & Hit)
{
	if ((fDst1 * fDst2) >= 0.0f)
		return false;
	if (fDst1 == fDst2)
		return false;
	Hit = P1 + (P2 - P1) * (-fDst1 / (fDst2 - fDst1));
	return true;
}

bool CollisionManager::InBox(Vector3 Hit, Vector3 B1, Vector3 B2, const int Axis)
{

	if (Axis == 1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return true;
	if (Axis == 2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return true;
	if (Axis == 3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return true;
	return false;
}

bool CollisionManager::isLineIntersectAABB(EntityBase * entity, Vector3 & origin, Vector3 & dir)
{

	{
		Vector3 MinPt = (entity->collider->GetMinAABB());
		Vector3 MaxPt = (entity->collider->GetMaxAABB());
		dir.Normalize();
		Vector3 invDir(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);

		float lambdaMinX = (MinPt.x - origin.x) * invDir.x;
		float lambdaMaxX = (MaxPt.x - origin.x) * invDir.x;
		float lambdaMinY = (MinPt.y - origin.y) * invDir.y;
		float lambdaMaxY = (MaxPt.y - origin.y) * invDir.y;
		float lambdaMinZ = (MinPt.z - origin.z) * invDir.z;
		float lambdaMaxZ = (MaxPt.z - origin.z) * invDir.z;

		//Rearrange the max and min
		if (lambdaMinX > lambdaMaxX) std::swap(lambdaMinX, lambdaMaxX);
		if (lambdaMinY > lambdaMaxY) std::swap(lambdaMinY, lambdaMaxY);
		if (lambdaMinZ > lambdaMaxZ) std::swap(lambdaMinZ, lambdaMaxZ);

		if ((lambdaMinX > lambdaMaxY) || (lambdaMinY > lambdaMaxX))
			return false;

		if (lambdaMinY > lambdaMinX)
			lambdaMinX = lambdaMinY;

		if (lambdaMaxY < lambdaMaxX)
			lambdaMaxX = lambdaMaxY;

		if ((lambdaMinX > lambdaMaxZ) || (lambdaMinZ > lambdaMaxX))
			return false;
		if (lambdaMinZ > lambdaMinX)
			lambdaMinX = lambdaMinZ;
		if (lambdaMaxZ < lambdaMaxX)
			lambdaMaxX = lambdaMaxZ;

		//outNearPoint = origin + (lambdaMinX * dir);
		return true;
	}
}

//void CollisionManager::add_ground(Collidable * ground)
//{
//	this->ground = ground;
//}
//
//Collidable * CollisionManager::get_ground()
//{
//	return ground;
//}

void CollisionManager::RenderGrid()
{
	RenderHelper::DrawLine(CollisionManager::GetInstance()->posColliderChecks, Color(0, 1, 0));
	CollisionManager::GetInstance()->posColliderChecks.clear();
}
