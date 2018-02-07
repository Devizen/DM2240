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
#include "../LuaEditor/LuaEditor.h"
#include "../Lua/LuaInterface.h"

CMenuScene::CMenuScene()
{
}

CMenuScene::~CMenuScene()
{
}

void CMenuScene::Init()
{
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	MeshBuilder::GetInstance()->GenerateQuad("INTRO_BG", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("INTRO_BG")->textureID[0] = LoadTGA("Image//WORLD//W_WATER.tga");

	MeshBuilder::GetInstance()->GenerateText("TEXT", 16.f, 16.f);
	MeshBuilder::GetInstance()->GetMesh("TEXT")->textureID[0] = LoadTGA("Image//TEXT.tga");
}

void CMenuScene::Update(double dt)
{
	if (MouseController::GetInstance()->IsButtonReleased(0))
	{
		std::cout << "Loading Game" << std::endl;
		CLuaInterface::GetInstance()->functionMap[CLuaInterface::OBJECT]->Run();
		SceneManager::GetInstance()->SetActiveScene("Start");
	}
}

void CMenuScene::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	LuaEditor* luaEditor = LuaEditor::GetInstance();

	if (luaEditor->GetCompleteLoadProgress() > 0)
	{
		ms.PushMatrix();
		ms.Translate(0.f/*((halfWindowWidth * 2.f) * (luaEditor->GetCurrentLoadProgress() / luaEditor->GetCompleteLoadProgress()) - (halfWindowWidth * 2.f))*/, 0.f, -1.f);
		ms.Scale((halfWindowWidth * 2.f) * (luaEditor->GetCurrentLoadProgress() / luaEditor->GetCompleteLoadProgress()), halfWindowHeight * 0.25f, 1);
		RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("INTRO_BG"));
		ms.PopMatrix();

		MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
		modelStack.PushMatrix();
		modelStack.Translate(startPosition);
		modelStack.Scale(fontSize, fontSize, fontSize);
		RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), *LuaEditor::GetInstance()->GetMesage(), Color(1.f, 0.f, 0.f));
		std::cout << "RENDER" << std::endl;
		modelStack.PopMatrix();

		modelStack = GraphicsManager::GetInstance()->GetModelStack();
		modelStack.PushMatrix();
		modelStack.Translate(0.f, halfWindowHeight * 0.3f, 0.f);
		modelStack.Scale(fontSize * 2.f, fontSize * 2.f, fontSize * 2.f);
		RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), 
			std::to_string(static_cast<int>((luaEditor->GetCurrentLoadProgress() / luaEditor->GetCompleteLoadProgress()) * 100.f)), Color(1.f, 0.f, 0.f));
		std::cout << "I AM IN RENDER CMENUSCENE: " << static_cast<int>((luaEditor->GetCurrentLoadProgress() / luaEditor->GetCompleteLoadProgress()) * 100.f) << std::endl;
		modelStack.PopMatrix();
	}
}

void CMenuScene::Exit()
{
	MeshBuilder::GetInstance()->RemoveMesh("INTRO_BG");
	GraphicsManager::GetInstance()->DetachCamera();
}
