#pragma once

#include "Collider.h"
#include "../MyTimer.h"
#include <vector>
class EntityBase;
class LineCollider : public CCollider
{
	Vector3 pos;
	Vector3 dir;
	//end range
	float range;
	float thickness;
	//move speed;
	float speed;
public:
	void Init(Vector3 pos, Vector3 dir, float speed = 1.0f, float range = FLT_MAX, float thickness = 1.0f)
	{
		this->pos = pos;
		this->dir = dir;
		this->range = range;
		this->thickness = thickness;
		this->speed = speed;
	}

	Vector3 GetPos() { return pos; }
	Vector3 GetDir() { return dir; }
	float GetRange() { return range; }
	float GetThickness() { return thickness; }
	float GetSpeed() { return speed; }
	
};