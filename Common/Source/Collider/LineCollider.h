#pragma once

#include "Collider.h"
#include "../MyTimer.h"
#include <vector>
class EntityBase;
class LineCollider : public CCollider
{
	MyTimer duration;
	std::vector<EntityBase*> collidedEntity;
	Vector3 pos;
	Vector3 dir;
	float range;
public:
	void Init(float duration, Vector3 pos, Vector3 dir, float range = FLT_MAX)
	{
		this->duration.set_duration(duration);
		this->duration.reset_timer();
		this->pos = pos;
		this->dir = dir;
		this->range = range;
		this->collidedEntity.clear();
	}
};