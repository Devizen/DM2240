#include "CMenuScene.h"

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

#include "../PlayerInfo/PlayerInfo.h"

CMenuScene::CMenuScene()
{
}

CMenuScene::~CMenuScene()
{
}

void CMenuScene::Init()
{
	MouseController::GetInstance()->SetKeepMouseCentered(false);
	Application::GetInstance().ShowMouse(true);
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	MeshBuilder::GetInstance()->GenerateQuad("INTRO_BG", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("INTRO_BG")->textureID[0] = LoadTGA("Image//SKYPLANE.tga");
	MeshBuilder::GetInstance()->GenerateQuad("BUTTON_BACKGROUND", Color(0.8, 0.8, 0.8), 1.f);
	//MeshBuilder::GetInstance()->GetMesh("BUTTON_BACKGROUND")->textureID[0] = LoadTGA("Image//SKYPLANE.tga");
	MeshBuilder::GetInstance()->GenerateQuad("MENU_OPTION_BUTTON", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("MENU_OPTION_BUTTON")->textureID[0] = LoadTGA("Image//UI//optionbutton.tga");

	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;

	Button* startButton = new Button("Button");
	startButton->set_scale_x(100).set_scale_y(50).set_x(0).set_y(0);
	startButton->image = MeshBuilder::GetInstance()->GetMesh("BUTTON_BACKGROUND");
	uiObjList.push_back(startButton);

	Button* optionButton = new Button("OptionButton");
	optionButton->set_scale_x(100).set_scale_y(50).set_x(0).set_y(-100);
	optionButton->image = MeshBuilder::GetInstance()->GetMesh("MENU_OPTION_BUTTON");
	uiObjList.push_back(optionButton);

	//MeshBuilder::GetInstance()->GetMesh("INTRO_BG")->textureID[0] = LoadTGA("Image//WORLD//W_WATER.tga");

	MeshBuilder::GetInstance()->GenerateText("TEXT", 16.f, 16.f);
	MeshBuilder::GetInstance()->GetMesh("TEXT")->textureID[0] = LoadTGA("Image//TEXT.tga");

	isLoadingGame = false;

	CPlayerInfo::GetInstance()->Init();
}

void CMenuScene::Update(double dt)
{
	MouseController::GetInstance()->SetKeepMouseCentered(false);
	Application::GetInstance().ShowMouse(true);
	isLoadingGame = false;

	float cursor_x, cursor_y;
	MouseController::GetInstance()->GetMousePosition(cursor_x, cursor_y);
	//std::cout << cursor_x << " , " <<  cursor_y << std::endl;
	cursor_y = Application::GetInstance().GetWindowHeight() - cursor_y;

	//because 0,0 is middle of screen
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;
	cursor_x -= halfWindowWidth;
	cursor_y -= halfWindowHeight;

	for (UILIST::iterator it = uiObjList.begin(); it != uiObjList.end(); ++it)
	{
		Button* button = dynamic_cast<Button*>(*it);
		if (button)
		{
			if (button->CheckCollision(cursor_x, cursor_y))
			{ 
				if (MouseController::GetInstance()->IsButtonReleased(0))
				{
					if (button->name == "Button")
					{
						std::cout << "Loading Game" << std::endl;
						isLoadingGame = true;
						CLuaInterface::GetInstance()->functionMap[CLuaInterface::OBJECT]->Run();
						SceneManager::GetInstance()->PushScene("Start");
						SceneManager::GetInstance()->PushMessage("Menu", SceneManager::MESSAGE::STOPRENDER);
					}
					if (button->name == "OptionButton")
					{
						std::cout << "Loading Option" << std::endl;
						SceneManager::GetInstance()->PushScene("Option");
						SceneManager::GetInstance()->PushMessage("Menu", SceneManager::MESSAGE::STOPRENDER);
					}
				}

			}
		}
		
	}
}

void CMenuScene::Render()
{
	if (SceneManager::GetInstance()->IsSceneAtBottom(this))
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, static_cast<GLsizei>(Application::GetInstance().GetWindowWidth()), static_cast<GLsizei>(Application::GetInstance().GetWindowHeight()));

	GraphicsManager::GetInstance()->SetPerspectiveProjection(CameraManager::GetInstance()->GetFrustumFoV(),
		CameraManager::GetInstance()->GetFrustumAspect(), CameraManager::GetInstance()->GetNearPlane(), CameraManager::GetInstance()->GetFarPlane());
	//GraphicsManager::GetInstance()->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(CameraManager::GetInstance()->GetActiveCam());

	//Render all 3D
	GraphicsManager::GetInstance()->SetOrthographicProjection(0, Application::GetInstance().GetWindowWidth(), 0, Application::GetInstance().GetWindowHeight(), -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();

	MS& ms = GraphicsManager::GetInstance()->GetModelStack();
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();

	/*Render loading text out.*/
	/*Render Lua Script out.*/
	float fontSize = (halfWindowWidth / halfWindowHeight) * 10.f;
	float halfFontSize = fontSize * 0.5f;
	/*Make the position start from left top.*/
	Vector3 startPosition(-halfWindowWidth, 0, 0.f);

	size_t tempSize = LuaEditor::GetInstance()->GetLine().size();

	static LuaEditor* luaEditor = LuaEditor::GetInstance();

	if (!isLoadingGame)
	{
		ms.PushMatrix();
		ms.Translate(0, 0, -1.f);
		ms.Scale(halfWindowWidth * 2.f, halfWindowHeight * 2.f, 1.f);
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("INTRO_BG"));
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
	}
	else if (luaEditor->GetCompleteLoadProgress() > 0)
	{
		/*Render loading text out.*/
		/*Render Lua Script out.*/
		float fontSize = (halfWindowWidth / halfWindowHeight) * 10.f;
		float halfFontSize = fontSize * 0.5f;
		/*Make the position start from left.*/
		Vector3 startPosition(-halfWindowWidth, 0, 0.f);

		size_t tempSize = LuaEditor::GetInstance()->GetLine().size();

		MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
		/*Loading Bar.*/
		ms.PushMatrix();
		/*Anchored at left position.*/
		ms.Translate((luaEditor->GetCurrentLoadProgress() / luaEditor->GetCompleteLoadProgress()) * halfWindowWidth, 0.f, 0.f);
		ms.Translate(-halfWindowWidth, 0.f, -1.f);
		ms.Scale((halfWindowWidth * 2.f) * (luaEditor->GetCurrentLoadProgress() / luaEditor->GetCompleteLoadProgress()), halfWindowHeight * 0.25f, 1.f);
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("INTRO_BG"));
		ms.PopMatrix();

		/*Loading Text.*/
		modelStack.PushMatrix();
		modelStack.Translate(-halfWindowWidth, -halfWindowHeight * 0.2f, 0.f);
		modelStack.Scale(fontSize, fontSize, fontSize);
		RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), *LuaEditor::GetInstance()->GetMesage(), Color(1.f, 0.f, 0.f));
		std::cout << "RENDERING in MENU SCENE." << std::endl;
		modelStack.PopMatrix();

		/*Loading Percentage.*/
		modelStack = GraphicsManager::GetInstance()->GetModelStack();
		modelStack.PushMatrix();
		modelStack.Translate(0.f, halfWindowHeight * 0.3f, 0.f);
		modelStack.Scale(fontSize * 2.f, fontSize * 2.f, fontSize * 2.f);
		RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), 
			std::to_string(static_cast<int>((luaEditor->GetCurrentLoadProgress() / luaEditor->GetCompleteLoadProgress()) * 100.f)) + "%",
			Color(1.f, 0.f, 0.f));
		std::cout << "I AM IN RENDER CMENUSCENE: " << static_cast<int>((luaEditor->GetCurrentLoadProgress() / luaEditor->GetCompleteLoadProgress()) * 100.f) << std::endl;
		modelStack.PopMatrix();
	}
}

void CMenuScene::Exit()
{
	MeshBuilder::GetInstance()->RemoveMesh("INTRO_BG");
	MeshBuilder::GetInstance()->RemoveMesh("MENU_OPTION_BUTTON");
	GraphicsManager::GetInstance()->DetachCamera();

	MouseController::GetInstance()->SetKeepMouseCentered(true);
	Application::GetInstance().ShowMouse(false);
	
	isLoadingGame = false;
}
