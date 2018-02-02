#include "CIntroScene.h"

#include "GL\glew.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "MeshBuilder.h"
#include "LoadTGA.h"
#include "../Application.h"
#include "KeyboardController.h"
#include "SceneManager.h"
#include "../QuadTree/CameraManager.h"
#include "../DepthFBO/DepthFBO.h"

#include "../Light.h"
#include "../SpatialPartition/SpatialPartitionManager.h"
#include "Manager\CollisionManager.h"
#include "../QuadTree/QuadTreeManager.h"
#include "Mesh.h"

CIntroScene::CIntroScene()
{
}

CIntroScene::~CIntroScene()
{
}

void CIntroScene::Init()
{
	glClearColor(0, 0, 0, 0);
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	MeshBuilder::GetInstance()->GenerateQuad("INTRO_BG", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("INTRO_BG")->textureID[0] = LoadTGA("Image//SKYPLANE.tga");
	//background = Create::Sprite2DObject("INTRO_BG", Vector3(halfWindowWidth, halfWindowHeight, 0.f),
	//	Vector3(halfWindowWidth * 2.f, halfWindowHeight * 2.f, 0.f));

	//CameraManager::GetInstance()->
	std::cout << "Loading Intro" << std::endl;
}

void CIntroScene::Update(double dt)
{
	if (KeyboardController::GetInstance()->IsKeyReleased(VK_SPACE))
	{
		std::cout << "Loading Menu" << std::endl;
		SceneManager::GetInstance()->SetActiveScene("Menu");
	}
}

void CIntroScene::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;

	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	//GraphicsManager::GetInstance()->AttachCamera(&camera);
	GraphicsManager::GetInstance()->DetachCamera();


	MS& ms = GraphicsManager::GetInstance()->GetModelStack();
	ms.LoadIdentity();

	ms.PushMatrix();
	ms.Translate(halfWindowWidth, halfWindowHeight, 0);
	ms.Scale(halfWindowWidth * 2.f, halfWindowHeight * 2.f, 1);
	RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("INTRO_BG"));
	ms.PopMatrix();
	
}

void CIntroScene::Exit()
{
	MeshBuilder::GetInstance()->RemoveMesh("INTRO_BG");
	GraphicsManager::GetInstance()->DetachCamera();
}
