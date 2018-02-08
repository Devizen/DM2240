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

	enum STATUS
	{
		PLACEOBJECT = 0,
		PLACEWAYPOINT
	} status;

	bool SpawnObject(TYPE _type, Vector3 _position);

	void SetEditorMode(const bool _editorMode) { editorMode = _editorMode; }
	const bool GetEditorMode(void) { return editorMode; }
private:
	MapEditor(void) : editorMode(false), status(PLACEOBJECT) {};
	static MapEditor* s_instance;

	bool editorMode;
};

#endif