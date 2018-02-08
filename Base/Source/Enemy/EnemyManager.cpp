#include "EnemyManager.h"
#include "Monk\Monk.h"
#include <algorithm>
#include "../PlayerInfo/ScoreManager.h"
#include "../PlayerInfo/PlayerInfo.h"

#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "MeshBuilder.h"

CEnemyManager* CEnemyManager::s_instance = 0;
CEnemyManager::CEnemyManager(void) :
	renderAABB(false),
	nearbyEnemies(0)
{
}

CEnemyManager::~CEnemyManager(void)
{
}

CEnemyManager * CEnemyManager::GetInstance(void)
{
	if (s_instance == nullptr)
		s_instance = new CEnemyManager();
	return s_instance;
}

void CEnemyManager::Update(double dt)
{
	nearbyEnemies = 0;
	for (std::vector<EntityBase*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		CMonk* monk = dynamic_cast<CMonk*>(*it);
		//Calculate if near player
		Vector3 dir = CPlayerInfo::GetInstance()->GetPos() - monk->GetCore()->GetPosition();
		if (dir.LengthSquared() < CHASE_DISTANCE * CHASE_DISTANCE)
			++nearbyEnemies;
	}



	for (std::vector<EntityBase*>::iterator it = enemyList.begin(); it != enemyList.end();) {
		if (dynamic_cast<CMonk*>(*it)->GetPartSize()) {
			dynamic_cast<CMonk*>(*it)->UpdatePart(dt);
			++it;
		}
		else
		{
			it = enemyList.erase(it);
			CScoreManager::GetInstance()->AddScore(2);
		}
	}
}

void CEnemyManager::AddEnemy(EntityBase * _enemy)
{
	enemyList.push_back(_enemy);
}

void CEnemyManager::RemoveEnemy(EntityBase * _enemy)
{
	for (std::vector<EntityBase*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		CMonk * monk = static_cast<CMonk*>(*it);
		if (monk->IsDone())
			continue;
		if (monk->GetEntityPart("MONK_HEAD") == _enemy)
		{
			Vector3 pos = monk->GetCore()->GetPosition();
			//it = enemyList.erase(it, enemyList.end());
			monk->SetIsDone(true);

			TriggerFear(pos);

			return;
		}
	}
	//std::vector<EntityBase*>::iterator it = std::remove(enemyList.begin(), enemyList.end(), _enemy);
	//if (it == enemyList.end())
	//	return;

}

void CEnemyManager::SpawnMonk(Vector3 pos)
{
	Create::Monk(pos, Vector3(5.f, 5.f, 5.f), CPlayerInfo::GetInstance());
}

bool CEnemyManager::GetShouldAllChase()
{
	return nearbyEnemies > 2;
}

void CEnemyManager::RenderStates()
{
	for (auto & e : enemyList)
	{
		if (e->IsDone())
			continue;
		CMonk * monk = static_cast<CMonk*>(e);
		MS& ms = GraphicsManager::GetInstance()->GetModelStack();
		Vector3 corePos = monk->GetCore()->GetPosition();
		Vector3 monkToMe = -corePos + CPlayerInfo::GetInstance()->GetPos();
		float dist = monkToMe.Length();
		Vector3 defaultFacing(0, 0, 1);
		float angle = 0.f;
		Vector3 rotateAboutAxis;
		try {
			rotateAboutAxis = defaultFacing.Cross(Vector3(monkToMe.x, 0, monkToMe.z).Normalize()).Normalize();
			angle = Math::RadianToDegree(acos(defaultFacing.Dot(monkToMe.Normalize())));
		}
		catch (DivideByZero e) {
			rotateAboutAxis.Set(0, 1, 0);
		}
		std::string text = "Patrol";
		switch (monk->GetState())
		{
		case CMonk::CHASE:
			text = "Chase";
			break;
		case CMonk::FLEE:
			text = "FLEE";
			break;
		}

		ms.PushMatrix();
		ms.Translate(corePos.x, corePos.y + 20, corePos.z);
		ms.Rotate(Math::RadianToDegree(atan2(monkToMe.x, monkToMe.z)), 0, 1, 0);
		ms.Scale(2 * dist / 100.f, 2 * dist / 100.f, 1);
		RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), text, Color(0, 0, 0));
		ms.PopMatrix();
	}
}

void CEnemyManager::TriggerFear(Vector3 pos)
{
	for (std::vector<EntityBase*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it)
	{
		CMonk* monk = static_cast<CMonk*>(*it);
		if (monk->IsDone())
			continue;
		if ((monk->GetCore()->GetPosition() - pos).Length() <= FEAR_DISTANCE)
		{
			monk->SetState(CMonk::STATES::FLEE);
		}
	}
}
