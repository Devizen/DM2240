#ifndef SCENE_TEXT_H
#define SCENE_TEXT_H

#include "Scene.h"
#include "Mtx44.h"
#include "PlayerInfo/PlayerInfo.h"
#include "GroundEntity.h"
#include "FPSCamera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "GenericEntity.h"
#include <vector>
/*Audio Player for music.*/
#include "AudioPlayer\AudioPlayer.h"

class ShaderProgram;
class SceneManager;
class TextEntity;
class Light;

class SceneText : public Scene
{	
public:
	SceneText();
	~SceneText();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();
	void RenderPassGPass(void);
	void RenderPassMain(void);
	void RenderWorld(void);
	void Spawner(std::string _name);
private:
	SceneText(SceneManager* _sceneMgr); // This is used to register to SceneManager

	//ShaderProgram* currProg;
	/*Shadow*/
	ShaderProgram* m_gPassShaderID;

	CPlayerInfo* playerInfo;
	GroundEntity* groundEntity;
	FPSCamera* camera;
	TextEntity* textObj[25];
	Light* lights[2];

	GenericEntity* theCube;

	//static SceneText* sInstance; // The pointer to the object that gets registered

	std::vector<unsigned char>heightMap;

	/*Size of Map for misc. calculations.*/
	float groundScale;

	/*Audio player.*/
	irrklang::ISoundEngine* audioPlayer;
};

#endif