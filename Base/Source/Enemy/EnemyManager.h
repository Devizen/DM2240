#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H
#include "EntityBase.h"
#include <vector>

#define CHASE_DISTANCE 100
#define UNCHASE_DISTANCE 110

class CEnemyManager {
	static CEnemyManager* s_instance;
public:
	CEnemyManager(void);
	~CEnemyManager(void);
	static CEnemyManager* GetInstance(void);

	void Update(double dt);
	void AddEnemy(EntityBase* _enemy);
	void RemoveEnemy(EntityBase* _enemy);
	void SetRenderAABB(bool _renderAABB) { renderAABB = _renderAABB; }
	bool GetRenderAABB(void) { return renderAABB; }
	void ToggleRenderAABB(void) { renderAABB = renderAABB ? false : true; }
	void SpawnMonk(Vector3 pos);
	std::vector<EntityBase*>& GetEnemyList(void) { return enemyList; }
	bool GetShouldAllChase();
	void RenderStates();
private:
	std::vector<EntityBase*>enemyList;
	bool renderAABB;
protected:
	int nearbyEnemies;
};

#endif