#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H
#include "EntityBase.h"
#include <vector>

#define CHASE_DISTANCE 100
#define UNCHASE_DISTANCE 110
#define FLEE_DISTANCE 120
#define FEAR_DISTANCE 80

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
	void TriggerFear(Vector3 pos);
	int nearbyEnemies;
private:
	std::vector<EntityBase*>enemyList;
	bool renderAABB;
protected:
};

#endif