#include "CVideoConfigScene.h"

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

CVideoConfigScene::CVideoConfigScene()
{
}

CVideoConfigScene::~CVideoConfigScene()
{
}

void CVideoConfigScene::Init()
{
	MouseController::GetInstance()->SetKeepMouseCentered(false);
	Application::GetInstance().ShowMouse(true);
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	MeshBuilder::GetInstance()->GenerateQuad("VC_OPTION_BACKGROUND", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("VC_OPTION_BACKGROUND")->textureID[0] = LoadTGA("Image//SKYPLANE.tga");
	MeshBuilder::GetInstance()->GenerateQuad("VC_OPTION_CLOSE_BUTTON", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("VC_OPTION_CLOSE_BUTTON")->textureID[0] = LoadTGA("Image//UI//closebutton.tga");

	MeshBuilder::GetInstance()->GenerateQuad("VC_RESO1", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("VC_RESO1")->textureID[0] = LoadTGA("Image//UI//800x600.tga");
	MeshBuilder::GetInstance()->GenerateQuad("VC_RESO2", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("VC_RESO2")->textureID[0] = LoadTGA("Image//UI//1024x768.tga");
	MeshBuilder::GetInstance()->GenerateQuad("VC_RESO3", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("VC_RESO3")->textureID[0] = LoadTGA("Image//UI//fullscreen.tga");
	MeshBuilder::GetInstance()->GenerateQuad("VC_RESO4", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("VC_RESO4")->textureID[0] = LoadTGA("Image//UI//windowed.tga");
	MeshBuilder::GetInstance()->GenerateQuad("VC_RESO5", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("VC_RESO5")->textureID[0] = LoadTGA("Image//UI//1920x1080.tga");

	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;

	background = MeshBuilder::GetInstance()->GetMesh("VC_OPTION_BACKGROUND");

	Button* startButton = new Button("Button");
	startButton->set_scale_x(30).set_scale_y(30).set_x(halfWindowWidth * 0.8f).set_y(halfWindowHeight * 0.8f);
	startButton->image = MeshBuilder::GetInstance()->GetMesh("VC_OPTION_CLOSE_BUTTON");
	uiObjList.push_back(startButton);

	Button* button = new Button("Reso1");
	button->set_scale_x(150).set_scale_y(80).set_x(-200).set_y(0);
	button->image = MeshBuilder::GetInstance()->GetMesh("VC_RESO1");
	uiObjList.push_back(button);

	button = new Button("Reso2");
	button->set_scale_x(150).set_scale_y(80).set_x(0).set_y(0);
	button->image = MeshBuilder::GetInstance()->GetMesh("VC_RESO2");
	uiObjList.push_back(button);

	button = new Button("Reso3");
	button->set_scale_x(150).set_scale_y(80).set_x(200).set_y(0);
	button->image = MeshBuilder::GetInstance()->GetMesh("VC_RESO5");
	uiObjList.push_back(button);

	button = new Button("Reso4");
	button->set_scale_x(150).set_scale_y(80).set_x(0).set_y(-200);
	if (Application::GetInstance().screenMode) //if is fullscreen, make option for window
		button->image = MeshBuilder::GetInstance()->GetMesh("VC_RESO4");
	else
		button->image = MeshBuilder::GetInstance()->GetMesh("VC_RESO3");
	uiObjList.push_back(button);

	//button = new Button("Reso4");
	//button->set_scale_x(150).set_scale_y(80).set_x(-100).set_y(-200);
	//button->image = MeshBuilder::GetInstance()->GetMesh("VC_RESO4");
	//uiObjList.push_back(button);
}

void CVideoConfigScene::Update(double dt)
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
						std::cout << "Closing Visual Config Option" << std::endl;
						SceneManager::GetInstance()->PopScene(this);
						SceneManager::GetInstance()->PushMessage("Option", SceneManager::MESSAGE::STARTRENDER);
					}
					else if (button->name == "Reso1")
					{
						//set to 800 x 600
						Application::GetInstance().SetResolution(800, 600);
					}
					else if (button->name == "Reso2")
					{
						//set to 1024 x 768
						Application::GetInstance().SetResolution(1024, 768);
					}
					else if (button->name == "Reso3")
					{
						//set to 1920x1080
						Application::GetInstance().SetResolution(1920, 1080);
					}
					else if (button->name == "Reso4")
					{
						//set fullscreen
						if (Application::GetInstance().screenMode)
						{
							Application::GetInstance().MakeWindowedMode();
							button->image = MeshBuilder::GetInstance()->GetMesh("VC_RESO3");
						}
						else
						{
							Application::GetInstance().MakeFullScreen();
							button->image = MeshBuilder::GetInstance()->GetMesh("VC_RESO4");
						}
					}
				}
			}
		}
	}
}

void CVideoConfigScene::Render()
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
}

void CVideoConfigScene::Exit()
{
	MeshBuilder::GetInstance()->RemoveMesh("VC_OPTION_BACKGROUND");
	MeshBuilder::GetInstance()->RemoveMesh("VC_OPTION_CLOSE_BUTTON");
	MeshBuilder::GetInstance()->RemoveMesh("VC_RESO1");
	MeshBuilder::GetInstance()->RemoveMesh("VC_RESO2");
	MeshBuilder::GetInstance()->RemoveMesh("VC_RESO3");
	MeshBuilder::GetInstance()->RemoveMesh("VC_RESO4");

	for (auto & b : uiObjList)
	{
		delete b;
	}
	uiObjList.clear();
}
