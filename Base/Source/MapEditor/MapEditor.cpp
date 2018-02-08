#include "MapEditor.h"

MapEditor* MapEditor::s_instance = 0;

MapEditor * MapEditor::GetInstance(void)
{
	if (s_instance == nullptr)
		s_instance = new MapEditor();
	return s_instance;
}

bool MapEditor::SpawnObject(TYPE _type, Vector3 _position)
{
	switch (_type)
	{
	case MONK:
		break;
	}
	return false;
}
