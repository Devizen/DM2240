#ifndef TOWER_MANAEGER_H
#define TOWER_MANAEGER_H
#include "../GenericEntity.h"
#include "Vector3.h"
#include <vector>

class CTowerManager : public GenericEntity {
	static CTowerManager* s_instance;
public:
	CTowerManager();
	~CTowerManager();

	static CTowerManager* GetInstance(void);
	void AddTower(GenericEntity* _tower);
	void RemoveTower(GenericEntity* _tower);
	std::vector<GenericEntity*>&GetTowerList(void);
private:
	std::vector<GenericEntity*>towerList;
protected:
};

namespace Create
{
	GenericEntity* Tower(const Vector3 & _position, const Vector3 & _scale);
};
#endif