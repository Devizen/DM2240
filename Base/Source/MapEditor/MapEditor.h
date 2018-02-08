#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include "Vector3.h"

class EntityBase;

class MapEditor
{
public:
	~MapEditor(void) {};
	static MapEditor* GetInstance(void);

	enum TYPE
	{
		NONE = 0,
		MONK,
		TOTAL
	};

	bool SpawnObject(TYPE _type, Vector3 _position);
private:
	MapEditor(void) {};
	static MapEditor* s_instance;
};

#endif