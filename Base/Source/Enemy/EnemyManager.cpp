#include "EnemyManager.h"
#include "Monk\Monk.h"
#include <algorithm>
#include "../PlayerInfo/ScoreManager.h"
#include "../PlayerInfo/PlayerInfo.h"

CEnemyManager* CEnemyManager::s_instance = 0;
CEnemyManager::CEnemyManager(void) :
	renderAABB(false)
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
	enemyList.erase(std::remove(enemyList.begin(), enemyList.end(), _enemy), enemyList.end());
}

void CEnemyManager::SpawnMonk(Vector3 pos)
{
	Create::Monk(pos, Vector3(5.f, 5.f, 5.f), CPlayerInfo::GetInstance());
}
