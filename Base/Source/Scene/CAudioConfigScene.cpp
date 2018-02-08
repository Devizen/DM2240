#include "CAudioConfigScene.h"

#include "GL\glew.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include "../Application.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "../QuadTree/CameraManager.h"
#include "../UI/Button.h"

#include "../LuaEditor/LuaEditor.h"
#include "../Lua/LuaInterface.h"

#include "../UI/TextField.h"
#include "../PlayerInfo/PlayerInfo.h"
#include "../AudioPlayer/AudioPlayer.h"

CAudioConfigScene::CAudioConfigScene()
{
}

CAudioConfigScene::~CAudioConfigScene()
{
}

void CAudioConfigScene::Init()
{
	MouseController::GetInstance()->SetKeepMouseCentered(false);
	Application::GetInstance().ShowMouse(true);
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	MeshBuilder::GetInstance()->GenerateQuad("AC_OPTION_BACKGROUND", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("AC_OPTION_BACKGROUND")->textureID[0] = LoadTGA("Image//SKYPLANE.tga");
	MeshBuilder::GetInstance()->GenerateQuad("AC_OPTION_CLOSE_BUTTON", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("AC_OPTION_CLOSE_BUTTON")->textureID[0] = LoadTGA("Image//UI//closebutton.tga");

	MeshBuilder::GetInstance()->GenerateQuad("VOLUMEUP", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("VOLUMEUP")->textureID[0] = LoadTGA("Image//UI//VOLUMEUP.tga");

	MeshBuilder::GetInstance()->GenerateQuad("VOLUMEDOWN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("VOLUMEDOWN")->textureID[0] = LoadTGA("Image//UI//VOLUMEDOWN.tga");

	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;

	background = MeshBuilder::GetInstance()->GetMesh("AC_OPTION_BACKGROUND");

	Button* startButton = new Button("Button");
	startButton->set_scale_x(30).set_scale_y(30).set_x(halfWindowWidth * 0.8f).set_y(halfWindowHeight * 0.8f);
	startButton->image = MeshBuilder::GetInstance()->GetMesh("AC_OPTION_CLOSE_BUTTON");
	uiObjList.push_back(startButton);

	Button* volumeUp = new Button("VOLUMEUP");
	volumeUp->set_scale_x(30).set_scale_y(30).set_x(halfWindowWidth * 0.4f).set_y(0.f);
	volumeUp->image = MeshBuilder::GetInstance()->GetMesh("VOLUMEUP");
	uiObjList.push_back(volumeUp);

	Button* volumeDown = new Button("VOLUMEDOWN");
	volumeDown->set_scale_x(30).set_scale_y(30).set_x(-halfWindowWidth * 0.4f).set_y(0.f);
	volumeDown->image = MeshBuilder::GetInstance()->GetMesh("VOLUMEDOWN");
	uiObjList.push_back(volumeDown);

	/*Audio Initialisation.*/
	static irrklang::ISoundEngine* audioPlayer = CAudioPlayer::GetInstance()->GetISoundEngine();
	audioPlayer->play2D("Audio/BGM/VOLUME.mp3", true);

	luaL_dofile(CLuaInterface::GetInstance()->luaState, "Lua//LuaVolume.lua");
	lua_getglobal(CLuaInterface::GetInstance()->luaState, "volumeLevel");
	audioPlayer->setSoundVolume(lua_tonumber(CLuaInterface::GetInstance()->luaState, -1));
}

void CAudioConfigScene::Update(double dt)
{
	static irrklang::ISoundEngine* audioPlayer = CAudioPlayer::GetInstance()->GetISoundEngine();
	static lua_State* luaState = CLuaInterface::GetInstance()->luaState;

	float cursor_x, cursor_y;
	MouseController::GetInstance()->GetMousePosition(cursor_x, cursor_y);
	//std::cout << cursor_x << " , " <<  cursor_y << std::endl;
	cursor_y = Application::GetInstance().GetWindowHeight() - cursor_y;

	//because 0,0 is middle of screen
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;
	cursor_x -= halfWindowWidth; /// coz 0,0 is middle
	cursor_y -= halfWindowHeight;

	if (KeyboardController::GetInstance()->IsKeyPressed(VK_BACK))
	{
		std::cout << "Closing Audio Config Option" << std::endl;
		SceneManager::GetInstance()->PopScene(this);
		SceneManager::GetInstance()->PushMessage("Option", SceneManager::MESSAGE::STARTRENDER);
	}

	for (UILIST::iterator it = uiObjList.begin(); it != uiObjList.end(); ++it)
	{
		Button* button = dynamic_cast<Button*>(*it);
		if (button)
		{
			button->x = button->defaultX * (halfWindowHeight * 0.005f);
			button->y = button->defaultY * (halfWindowHeight * 0.005f);

			button->scale_x = button->defaultScaleX * (halfWindowHeight * 0.005f);
			button->scale_y = button->defaultScaleY * (halfWindowHeight * 0.005f);

			if (button->CheckCollision(cursor_x, cursor_y))
			{
				if (MouseController::GetInstance()->IsButtonReleased(0))
				{
					if (button->name == "Button")
					{
						std::cout << "Closing Audio Config Option" << std::endl;
						SceneManager::GetInstance()->PopScene(this);
						SceneManager::GetInstance()->PushMessage("Option", SceneManager::MESSAGE::STARTRENDER);
					}
				}
				if (MouseController::GetInstance()->IsButtonDown(0))
				{
					if (button->name.compare("VOLUMEUP") == 0)
					{
						if (audioPlayer->getSoundVolume() >= 1.f)
							continue;
						std::cout << "<Adjusting Volume Up.>" << std::endl;
						audioPlayer->setSoundVolume(audioPlayer->getSoundVolume() + dt * 0.25f);

						//lua_getglobal(luaState, "UpdateVolumeLevel");
						//lua_pushnumber(luaState, audioPlayer->getSoundVolume());
						//lua_call(luaState, 1, 0);
						lua_getglobal(luaState, "SaveVolume");
						char outputString[80];
						sprintf(outputString, "%s = %6.4f\n", "volumeLevel", static_cast<float>(audioPlayer->getSoundVolume()));
						lua_pushstring(luaState, outputString);
						lua_pushinteger(luaState, true);
						lua_call(luaState, 2, 0);
					}
					else if (button->name.compare("VOLUMEDOWN") == 0)
					{
						if (audioPlayer->getSoundVolume() <= 0.f)
							continue;
						std::cout << "<Adjusting Volume Down.>" << std::endl;
						audioPlayer->setSoundVolume(audioPlayer->getSoundVolume() - dt * 0.25f);
						lua_getglobal(luaState, "SaveVolume");
						char outputString[80];
						sprintf(outputString, "%s = %6.4f\n", "volumeLevel", static_cast<float>(audioPlayer->getSoundVolume()));
						lua_pushstring(luaState, outputString);
						lua_pushinteger(luaState, true);
						lua_call(luaState, 2, 0);
						//lua_pushnumber(luaState, audioPlayer->getSoundVolume());
						//lua_setglobal(luaState, "UpdateVolumeLevel");
						//char outputString[80];
						//sprintf(outputString, "%s = %6.4f\n", "volumeLevel", audioPlayer->getSoundVolume());
						//lua_pushstring(luaState, outputString);
						//lua_pushnumber(luaState, audioPlayer->getSoundVolume());
						//lua_call(luaState, 1, 0);
					}
				}
			}
		}
	}
}

void CAudioConfigScene::Render()
{
	if (SceneManager::GetInstance()->IsSceneAtBottom(this))
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, static_cast<GLsizei>(Application::GetInstance().GetWindowWidth()), static_cast<GLsizei>(Application::GetInstance().GetWindowHeight()));

	GraphicsManager::GetInstance()->SetPerspectiveProjection(CameraManager::GetInstance()->GetFrustumFoV(),
		CameraManager::GetInstance()->GetFrustumAspect(), CameraManager::GetInstance()->GetNearPlane(), CameraManager::GetInstance()->GetFarPlane());
	//GraphicsManager::GetInstance()->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(CameraManager::GetInstance()->GetActiveCam());

	//Render all 3D
	//GraphicsManager::GetInstance()->SetOrthographicProjection(0, Application::GetInstance().GetWindowWidth(), 0, Application::GetInstance().GetWindowHeight(), -10, 10);
	//GraphicsManager::GetInstance()->DetachCamera();

	MS& ms = GraphicsManager::GetInstance()->GetModelStack();
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();

	ms.PushMatrix();
	ms.Translate(0, 0, 0);
	ms.Scale(halfWindowWidth * 2.f, halfWindowHeight * 2.f, 1);
	RenderHelper::RenderMesh(background);
	ms.PopMatrix();

	for (UILIST::iterator it = uiObjList.begin(); it != uiObjList.end(); ++it)
	{
		Button* button = dynamic_cast<Button*>(*it);
		if (button)
		{
			ms.PushMatrix();
			ms.Translate(button->x, button->y, 2);
			ms.Scale(button->scale_x, button->scale_y, 1);
			RenderHelper::RenderMesh(button->image);
			ms.PopMatrix();
		}
	}

	ms.PushMatrix();
	const char wad[] = "Volume Control";
	int len = strlen(wad);
	ms.Translate(0.f - (halfWindowWidth * 0.02f * len), halfWindowHeight * 0.3f, 1.f);
	ms.Scale(halfWindowHeight * 0.075f, halfWindowHeight * 0.075f, 1);
	RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), wad, Color(0.f, 0.f, 0.f));
	ms.PopMatrix();

	ms.PushMatrix();
	std::string volume = std::to_string(static_cast<int>(CAudioPlayer::GetInstance()->GetISoundEngine()->getSoundVolume() * 100));
	size_t tempLength = volume.length();
	std::cout << tempLength << std::endl;
	ms.Translate(0.f - (halfWindowWidth * 0.02f * tempLength), 0.f, 1.f);
	ms.Scale(halfWindowHeight * 0.1f, halfWindowHeight * 0.1f, 1);
	RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), volume, Color(0.f, 0.f, 0.f));
	ms.PopMatrix();
}

void CAudioConfigScene::Exit()
{
	MeshBuilder::GetInstance()->RemoveMesh("AC_OPTION_BACKGROUND");
	MeshBuilder::GetInstance()->RemoveMesh("AC_OPTION_CLOSE_BUTTON");
}
