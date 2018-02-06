#include "CPauseScene.h"

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

CPauseScene::CPauseScene()
{
}

CPauseScene::~CPauseScene()
{
}

void CPauseScene::Init()
{
	MouseController::GetInstance()->SetKeepMouseCentered(false);
	Application::GetInstance().ShowMouse(true);
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	MeshBuilder::GetInstance()->GenerateQuad("PAUSE_BACKGROUND", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("PAUSE_BACKGROUND")->textureID[0] = LoadTGA("Image//SKYPLANE.tga");
	MeshBuilder::GetInstance()->GenerateQuad("PAUSE_CLOSE_BUTTON", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("PAUSE_CLOSE_BUTTON")->textureID[0] = LoadTGA("Image//UI//closebutton.tga");
	MeshBuilder::GetInstance()->GenerateQuad("PAUSE_OPTION_BUTTON", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("PAUSE_OPTION_BUTTON")->textureID[0] = LoadTGA("Image//UI//optionbutton.tga");
	MeshBuilder::GetInstance()->GenerateQuad("PAUSE_HOME_BUTTON", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("PAUSE_HOME_BUTTON")->textureID[0] = LoadTGA("Image//UI//homebutton.tga");
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;

	Button* startButton = new Button("Button");
	startButton->set_scale_x(30).set_scale_y(30).set_x(halfWindowWidth * 0.8f).set_y(halfWindowHeight * 0.8f);
	startButton->image = MeshBuilder::GetInstance()->GetMesh("PAUSE_CLOSE_BUTTON");
	uiObjList.push_back(startButton);

	Button* optionButton = new Button("Option");
	optionButton->set_scale_x(50).set_scale_y(50).set_x(0).set_y(0);
	optionButton->image = MeshBuilder::GetInstance()->GetMesh("PAUSE_OPTION_BUTTON");
	uiObjList.push_back(optionButton);

	Button* homeButton = new Button("Home");
	homeButton->set_scale_x(50).set_scale_y(50).set_x(halfWindowWidth * -0.8f).set_y(halfWindowHeight * 0.8f);
	homeButton->image = MeshBuilder::GetInstance()->GetMesh("PAUSE_HOME_BUTTON");
	uiObjList.push_back(homeButton);
	//MeshBuilder::GetInstance()->GenerateText("TEXT", 16.f, 16.f);
	//MeshBuilder::GetInstance()->GetMesh("TEXT")->textureID[0] = LoadTGA("Image//TEXT.tga");
}

void CPauseScene::Update(double dt)
{
	float cursor_x, cursor_y;
	MouseController::GetInstance()->GetMousePosition(cursor_x, cursor_y);
	//std::cout << cursor_x << " , " <<  cursor_y << std::endl;
	cursor_y = Application::GetInstance().GetWindowHeight() - cursor_y;

	//because 0,0 is middle of screen
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;
	cursor_x -= halfWindowWidth; /// coz 0,0 is middle
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
						std::cout << "Closing Pause" << std::endl;
						SceneManager::GetInstance()->PopScene("Pause");
					}
					if (button->name == "Option")
					{
						std::cout << "Opening Option" << std::endl;
						SceneManager::GetInstance()->PushScene("Option");
					}
					if (button->name == "Home")
					{
						std::cout << "Going Back To Main Menu" << std::endl;
						SceneManager::GetInstance()->PopToScene("Menu");
					}
				}
				

			}
		}

	}

	if (KeyboardController::GetInstance()->IsKeyPressed(VK_HOME))
		SceneManager::GetInstance()->PrintSceneStackInfo();
}

void CPauseScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("PAUSE_BACKGROUND"));
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

void CPauseScene::Exit()
{
	MouseController::GetInstance()->SetKeepMouseCentered(true);
	Application::GetInstance().ShowMouse(false);

	MeshBuilder::GetInstance()->RemoveMesh("PAUSE_BACKGROUND");
	MeshBuilder::GetInstance()->RemoveMesh("PAUSE_CLOSE_BUTTON");
	MeshBuilder::GetInstance()->RemoveMesh("PAUSE_OPTION_BUTTON");
	MeshBuilder::GetInstance()->RemoveMesh("PAUSE_HOME_BUTTON");

	for (auto & uiobj : uiObjList)
		delete uiobj;
	uiObjList.clear();

	//GraphicsManager::GetInstance()->DetachCamera();
}
