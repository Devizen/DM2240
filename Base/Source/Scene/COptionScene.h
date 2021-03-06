#pragma once

#include "Scene.h"
#include "Mtx44.h"
#include "../FPSCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include <vector>
/*Audio Player for music.*/
//#include "../AudioPlayer\AudioPlayer.h"
//#include "../SpriteEntity.h"
#include "../UI/UIObj.h"
#include "../Component/Option.h"

class COptionScene : public Scene
{
public:
	COptionScene();
	~COptionScene();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
private:
	FPSCamera camera;
	//SpriteEntity* background;
	typedef std::vector<UIObj*> UILIST;
	UILIST uiObjList;

	//OptionBase<Key>* saveFile;
};