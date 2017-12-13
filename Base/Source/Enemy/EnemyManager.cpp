#include "EnemyManager.h"
#include "Monk\Monk.h"
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
	for (std::vector<EntityBase*>::iterator it = enemyList.begin(); it != enemyList.end(); ++it) {
		dynamic_cast<CMonk*>(*it)->UpdatePart(dt);
	}
}

void CEnemyManager::AddEnemy(EntityBase * _enemy)
{
	enemyList.push_back(_enemy);
}

void CEnemyManager::RemoveEnemy(EntityBase * _enemy)
{
	
}
