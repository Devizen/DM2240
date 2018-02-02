#pragma once
#ifndef SCENE_INTRO_H
#define SCENE_INTRO_H

#include "Scene.h"
#include "Mtx44.h"
#include "../FPSCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include <vector>
/*Audio Player for music.*/
#include "../AudioPlayer\AudioPlayer.h"
//#include "../SpriteEntity.h"

//class ShaderProgram;
//class SceneManager;
//class TextEntity;
//class Light;

class CIntroScene : public Scene
{
public:
	CIntroScene();
	~CIntroScene();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
private:
	FPSCamera camera;
	//SpriteEntity* background;
};

#endif