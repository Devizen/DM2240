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

	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;

	Button* startButton = new Button("Button");
	startButton->set_scale_x(100).set_scale_y(50).set_x(0).set_y(0);
	startButton->image = MeshBuilder::GetInstance()->GetMesh("BUTTON_BACKGROUND");
	uiObjList.push_back(startButton);

}

void CMenuScene::Update(double dt)
{
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
				if (button->name == "Button")
				{
					if (MouseController::GetInstance()->IsButtonReleased(0))
					{
						std::cout << "Loading Game" << std::endl;
						SceneManager::GetInstance()->SetActiveScene("Start");
					}
				}

			}
		}
	}
	//if (MouseController::GetInstance()->IsButtonReleased(0))
	//{
	//	std::cout << "Loading Game" << std::endl;
	//	SceneManager::GetInstance()->SetActiveScene("Start");
	//}
}

void CMenuScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->SetPerspectiveProjection(CameraManager::GetInstance()->GetFrustumFoV(),
		CameraManager::GetInstance()->GetFrustumAspect(), CameraManager::GetInstance()->GetNearPlane(), CameraManager::GetInstance()->GetFarPlane());
	//GraphicsManager::GetInstance()->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(CameraManager::GetInstance()->GetActiveCam());

	//Render all 3D



	MS& ms = GraphicsManager::GetInstance()->GetModelStack();
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();


	ms.PushMatrix();
	ms.Translate(0, 0, 0);
	ms.Scale(halfWindowWidth * 2.f, halfWindowHeight * 2.f, 1);
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

void CMenuScene::Exit()
{
	MeshBuilder::GetInstance()->RemoveMesh("INTRO_BG");
	GraphicsManager::GetInstance()->DetachCamera();

	MouseController::GetInstance()->SetKeepMouseCentered(true);
	Application::GetInstance().ShowMouse(false);
}
