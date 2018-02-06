#pragma once
#ifndef SCENE_MENU_H
#define SCENE_MENU_H

#include "Scene.h"
#include "Mtx44.h"
#include "../FPSCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include <vector>
/*Audio Player for music.*/
#include "../AudioPlayer\AudioPlayer.h"
//#include "../SpriteEntity.h"
#include "../UI/UIObj.h"

//class ShaderProgram;
//class SceneManager;
//class TextEntity;
//class Light;

class CMenuScene : public Scene
{
public:
	CMenuScene();
	~CMenuScene();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
private:
	FPSCamera camera;
	//SpriteEntity* background;
	typedef std::vector<UIObj*> UILIST;
	UILIST uiObjList;

	bool isLoadingGame;
};

#endif