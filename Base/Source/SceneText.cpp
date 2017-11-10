#include "SceneText.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "EntityManager.h"

#include "GenericEntity.h"
#include "GroundEntity.h"
#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"
#include "SkyBox/SkyBoxEntity.h"

/*Rendering objects.*/
#include "RenderHelper.h"
/*Terrain.*/
#include "Terrain\LoadHmap.h"
/*For categorising generic entity type.*/
#include "EntityTypes\EntityTypes.h"
/*Shadow*/
#include "DepthFBO\DepthFBO.h"
/*Spatial Partitioning*/
#include "SpatialPartition\SpatialPartition.h"
#include "SpatialPartition\SpatialPartitionManager.h"
#include "MasterEntityManager\MasterEntityManager.h"
/*Scene Graph.*/
#include "SceneGraph\SceneGraph.h"
#include <iostream>
using namespace std;

SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());

SceneText::SceneText()
{
}

SceneText::SceneText(SceneManager* _sceneMgr)
{
	_sceneMgr->AddScene("Start", this);
}

SceneText::~SceneText()
{
	CSceneGraph::GetInstance()->Destroy();
}

void SceneText::Init()
{
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Shadow.vertexshader", "Shader//Shadow.fragmentshader");
	m_gPassShaderID = GraphicsManager::GetInstance()->LoadShader("gpass", "Shader//GPass.vertexshader", "Shader//GPass.fragmentshader");
	
	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");
	
	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(-1, 1, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->name = "lights[0]";

	//lights[0] = new Light();
	//GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	//lights[0]->type = Light::LIGHT_DIRECTIONAL;
	//lights[0]->position.Set(0, 20, 0);
	//lights[0]->color.Set(1, 1, 1);
	//lights[0]->power = 1;
	//lights[0]->kC = 1.f;
	//lights[0]->kL = 0.01f;
	//lights[0]->kQ = 0.001f;
	//lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	//lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	//lights[0]->exponent = 3.f;
	//lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	//lights[0]->name = "lights[0]";

	lights[1] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[1]", lights[1]);
	lights[1]->type = Light::LIGHT_DIRECTIONAL;
	lights[1]->position.Set(1, 1, 0);
	lights[1]->color.Set(1, 1, 0.5f);
	lights[1]->power = 0.4f;
	lights[1]->name = "lights[1]";

	currProg->UpdateInt("numLights", 1);
	currProg->UpdateInt("textEnabled", 0);

	/*Fog*/
	Color fogColor(0.5f, 0.5f, 0.5f); //Vec3 Color
	currProg->UpdateVector3("fogParam.color", &fogColor.r);
	currProg->UpdateFloat("fogParam.start", 0);
	currProg->UpdateFloat("fogParam.end", 1000);
	currProg->UpdateFloat("fogParam.density", 0.002f);
	/*
	Fog Type 0
	Slightly foggy.
	Fog is between start and end.

	Fog Type 1
	Natural fog.
	As it goes further from position, the fog will be thicker.

	Fog Type 2
	Denser among all 3 types of Fog.
	Will look thicker at the further end.
	*/
	currProg->UpdateInt("fogParam.type", 1);
	currProg->UpdateInt("fogParam.enabled", 0);

	/*Shadow*/
	currProg->AddUniform("lightDepthMVP");
	currProg->AddUniform("shadowMap");

	m_gPassShaderID->AddUniform("lightDepthMVP");
	m_gPassShaderID->AddUniform("colorTextureEnabled[0]");
	m_gPassShaderID->AddUniform("colorTexture[0]");
	m_gPassShaderID->AddUniform("colorTextureEnabled[1]");
	m_gPassShaderID->AddUniform("colorTexture[1]");
	m_gPassShaderID->AddUniform("colorTextureEnabled[2]");
	m_gPassShaderID->AddUniform("colorTexture[2]");

	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();

	// Create and attach the camera to the scene
	//camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera.Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	playerInfo->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Load all the meshes
	MeshBuilder::GetInstance()->GenerateAxes("reference");
	MeshBuilder::GetInstance()->GenerateCrossHair("crosshair");
	MeshBuilder::GetInstance()->GenerateQuad("quad", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("quad")->textureID[0] = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GenerateLine("redline", Color(1, 0, 0));
	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID[0] = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);
	MeshBuilder::GetInstance()->GenerateOBJ("Chair", "OBJ//chair.obj");
	MeshBuilder::GetInstance()->GetMesh("Chair")->textureID[0] = LoadTGA("Image//chair.tga");
	MeshBuilder::GetInstance()->GenerateRing("ring", Color(1, 0, 1), 36, 1, 0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("lightball", Color(1, 1, 1), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateSphere("sphere", Color(1, 0, 0), 18, 36, 10.f);
	MeshBuilder::GetInstance()->GenerateCone("cone", Color(0.5f, 1, 0.3f), 36, 10.f, 10.f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kDiffuse.Set(0.99f, 0.99f, 0.99f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kSpecular.Set(0.f, 0.f, 0.f);
	MeshBuilder::GetInstance()->GenerateCube("cube", Color(1.0f, 1.0f, 1.0f), 1.0f);
	MeshBuilder::GetInstance()->GenerateQuad("GRASS_DARKGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GRASS_DARKGREEN")->textureID[0] = LoadTGA("Image//grass_darkgreen.tga");
	MeshBuilder::GetInstance()->GenerateQuad("GEO_GRASS_LIGHTGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GEO_GRASS_LIGHTGREEN")->textureID[0] = LoadTGA("Image//grass_lightgreen.tga");
	MeshBuilder::GetInstance()->GenerateQuad("W_GRASS", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("W_GRASS")->textureID[0] = LoadTGA("Image//WORLD//W_GRASS.tga");
	/*Generate Ray.*/
	MeshBuilder::GetInstance()->GenerateRay("RAY", 100.f);

	/*Combination Ground Tile.*/
	MeshBuilder::GetInstance()->GenerateQuad("COMGROUND", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("COMGROUND")->textureID[0] = LoadTGA("Image//WORLD//W_GRASS.tga");
	MeshBuilder::GetInstance()->GetMesh("COMGROUND")->textureID[1] = LoadTGA("Image//WORLD//W_SNOW.tga");

	/*3 different spheres to show Level of Detail.*/
	MeshBuilder::GetInstance()->GenerateSphere("GREENSPHERE", Color(0, 1, 0), 18, 36, 5.f);
	MeshBuilder::GetInstance()->GenerateSphere("BLUESPHERE", Color(0, 0, 1), 18, 36, 5.f);
	MeshBuilder::GetInstance()->GenerateSphere("REDSPHERE", Color(1, 0, 0), 18, 36, 5.f);

	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_FRONT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BACK", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_LEFT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_RIGHT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_TOP", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BOTTOM", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_FRONT")->textureID[0] = LoadTGA("Image//SkyBox//skybox_front.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BACK")->textureID[0] = LoadTGA("Image//SkyBox//skybox_back.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_LEFT")->textureID[0] = LoadTGA("Image//SkyBox//skybox_left.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_RIGHT")->textureID[0] = LoadTGA("Image//SkyBox//skybox_right.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_TOP")->textureID[0] = LoadTGA("Image//SkyBox//skybox_top.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BOTTOM")->textureID[0] = LoadTGA("Image//SkyBox//skybox_bottom.tga");

	/*Terrain.*/
	MeshBuilder::GetInstance()->GenerateTerrain("TERRAIN", "Image//TERRAIN//heightMap.raw", heightMap);
	MeshBuilder::GetInstance()->GetMesh("TERRAIN")->textureID[0] = LoadTGA("Image//WORLD//W_GRASS.tga");
	MeshBuilder::GetInstance()->GetMesh("TERRAIN")->textureID[1] = LoadTGA("Image//WORLD//W_SNOW.tga");
	MeshBuilder::GetInstance()->GetMesh("TERRAIN")->textureID[2] = LoadTGA("Image//WORLD//W_WATER.tga");

	/*Number of partitions for each X-axis and Z-axis.*/
	const unsigned numOfPartitionXZ = 5;
	const float groundScale(400.f);

	/*Create spatial partitions with ground size.*/
	CSpatialPartitionManager::GetInstance()->Init(numOfPartitionXZ, static_cast<unsigned>(groundScale), playerInfo);

	playerInfo->SetSpatialPartition(CSpatialPartitionManager::GetInstance()->UpdateGridInfo(playerInfo->GetPos()));
	//playerInfo->SetPartition(CSpatialPartitionManager::GetInstance()->GetPlayerGrid());
	/*playerInfo->SetMinBoundary(CSpatialPartitionManager::GetInstance()->GetPartition(playerInfo->GetPartition().front())->GetMinBoundary());
	playerInfo->SetMaxBoundary(CSpatialPartitionManager::GetInstance()->GetPartition(playerInfo->GetPartition().front())->GetMaxBoundary());*/


	/*Create the same amount of entity manager for each partition.*/
	//CMasterEntityManager::GetInstance()->Init(CSpatialPartitionManager::GetInstance()->GetPartitionCount());
	//CMasterEntityManager::GetInstance()->SetPartitionNum(playerInfo->GetPartition().front());

	for (size_t i = 0; i < CSpatialPartitionManager::GetInstance()->GetPartitionCount(); ++i)
	{
		Vector3 position = CSpatialPartitionManager::GetInstance()->GetPartition(i)->GetPosition();
		EntityBase* chair = Create::Entity("GREENSPHERE", position, Vector3(1.f, 1.f, 1.f));
		chair->SetEntityType(ECEntityTypes::OBJECT);
		//CMasterEntityManager::GetInstance()->AddEntity(chair);
	}

	//groundEntity = Create::Ground("GRASS_DARKGREEN", "GEO_GRASS_LIGHTGREEN");
	groundEntity = Create::Ground("COMGROUND", "COMGROUND");
	// Customise the ground entity
	groundEntity->SetPosition(Vector3(0, -10, 0));
	groundEntity->SetScale(Vector3(groundScale, groundScale, groundScale));
	groundEntity->SetGrids(Vector3(1.0f, 1.0f, 1.0f));
	groundEntity->SetEntityType(ECEntityTypes::STATIC);
	playerInfo->SetTerrain(groundEntity);

	GenericEntity* terrain = Create::Entity("TERRAIN", Vector3(0.f, -11.f, 0.f));
	terrain->SetEntityType(ECEntityTypes::STATIC);
	terrain->SetScale(Vector3(2000.f, 350.f, 2000.f));

	/*Shadow Quad.*/
	//MeshBuilder::GetInstance()->GenerateQuad("LIGHT_DEPTH_TEXTURE", Color(1, 1, 1), 1.f);
	//MeshBuilder::GetInstance()->GetMesh("LIGHT_DEPTH_TEXTURE")->textureID[0] = DepthFBO::GetInstance()->GetTexture();
	//GenericEntity* shadowQuad = Create::Entity("LIGHT_DEPTH_TEXTURE", Vector3(50.f, 0.f, 0.f));
	//shadowQuad->SetScale(Vector3(20.f, 20.f, 1.f));
	//shadowQuad->SetEntityType(ECEntityTypes::TERRAIN);

	//GenericEntity* aSphere = Create::Entity("sphere", Vector3(0.f, 20.f, 0.f));

	//MeshBuilder::GetInstance()->GenerateQuad("floor", Color(1, 1, 1), 1.f);
	//MeshBuilder::GetInstance()->GetMesh("floor")->textureID[0] = LoadTGA("Image//WORLD//W_WATER.tga");
	//GenericEntity* aQuad = Create::Entity("floor", Vector3(0.f, -5.f, 0.f));
	//aQuad->SetScale(Vector3(100.f, 100.f, 1.f));
	//aQuad->SetRotateAngle(-90.f);
	//aQuad->SetRotateAxis(Vector3(1.f, 0.f, 0.f));

	// Create entities into the scene
	//Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference
	//Create::Entity("lightball", Vector3(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z)); // Lightball
	GenericEntity* aCube = Create::Entity("cube", Vector3(-20.0f, 0.0f, -20.0f));
	GenericEntity* bCube = Create::Entity("cube", Vector3(-40.0f, 0.0f, -40.0f));

	/*Create the root node.*/
	CSceneNode* theNode = CSceneGraph::GetInstance()->AddNode(aCube);
	if (theNode == nullptr)
		std::cout << "FAILED TO ADD NODE" << std::endl;
	/*Add another node to the root node.*/
	CSceneNode* anotherNode = theNode->AddChild(aCube);
	if (theNode == nullptr)
		std::cout << "FAILED TO ADD NODE" << std::endl;
	theNode->PrintSelf();

//	Create::Text3DObject("text", Vector3(0.0f, 0.0f, 0.0f), "DM2210", Vector3(10.0f, 10.0f, 10.0f), Color(0, 1, 1));
	Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

	SkyBoxEntity* theSkyBox = Create::SkyBox("SKYBOX_FRONT", "SKYBOX_BACK",
											 "SKYBOX_LEFT", "SKYBOX_RIGHT",
											 "SKYBOX_TOP", "SKYBOX_BOTTOM");
	theSkyBox->SetEntityType(ECEntityTypes::STATIC);

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	for (int i = 0; i < 5; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f,0.0f,0.0f));
		textObj[i]->SetEntityType(ECEntityTypes::STATIC);
	}
	textObj[0]->SetText("HELLO WORLD");
}

void SceneText::Update(double dt)
{
	static float horizontal = lights[0]->position.x;
	static float vertical = lights[0]->position.y;
	static float frontback = lights[0]->position.z;

	if (KeyboardController::GetInstance()->IsKeyPressed(VK_NUMPAD4))
		horizontal -= 10.f * static_cast<float>(dt);
	if (KeyboardController::GetInstance()->IsKeyPressed(VK_NUMPAD6))
		horizontal += 10.f * static_cast<float>(dt);
	if (KeyboardController::GetInstance()->IsKeyPressed(VK_NUMPAD2))
		vertical -= 10.f * static_cast<float>(dt);
	if (KeyboardController::GetInstance()->IsKeyPressed(VK_NUMPAD8))
		vertical += 10.f * static_cast<float>(dt);

	lights[0]->position.Set(horizontal, vertical, frontback);
	// Update our entities
	EntityManager::GetInstance()->Update(dt);

	// THIS WHOLE CHUNK TILL <THERE> CAN REMOVE INTO ENTITIES LOGIC! Or maybe into a scene function to keep the update clean
	if(KeyboardController::GetInstance()->IsKeyDown('1'))
		glEnable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('2'))
		glDisable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(KeyboardController::GetInstance()->IsKeyDown('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	if(KeyboardController::GetInstance()->IsKeyDown('5'))
	{
		lights[0]->type = Light::LIGHT_POINT;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('6'))
	{
		lights[0]->type = Light::LIGHT_DIRECTIONAL;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('7'))
	{
		lights[0]->type = Light::LIGHT_SPOT;
	}

	if(KeyboardController::GetInstance()->IsKeyDown('I'))
		lights[0]->position.z -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('K'))
		lights[0]->position.z += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('J'))
		lights[0]->position.x -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('L'))
		lights[0]->position.x += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('O'))
		lights[0]->position.y -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('P'))
		lights[0]->position.y += (float)(10.f * dt);

	// if the left mouse button was released
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::LMB))
	{
		cout << "Left Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::RMB))
	{
		cout << "Right Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::MMB))
	{
		cout << "Middle Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) != 0.0)
	{
		cout << "Mouse Wheel has offset in X-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) << endl;
	}
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) != 0.0)
	{
		cout << "Mouse Wheel has offset in Y-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) << endl;
	}
	// <THERE>

	// Update the player position and other details based on keyboard and mouse inputs
	playerInfo->Update(dt);

	//camera.Update(dt); // Can put the camera into an entity rather than here (Then we don't have to write this)

	GraphicsManager::GetInstance()->UpdateLights(dt);

	// Update the 2 text object values. NOTE: Can do this in their own class but i'm lazy to do it now :P
	// Eg. FPSRenderEntity or inside RenderUI for LightEntity
	std::ostringstream ss;
	ss.precision(5);
	float fps = (float)(1.f / dt);
	ss << "FPS:" << fps;
	textObj[1]->SetText(ss.str());

	std::ostringstream ss1;
	ss1.precision(4);
	ss1 << "Player:" << playerInfo->GetPos();
	textObj[2]->SetText(ss1.str());

	if (playerInfo->GetSpatialPartition()->CheckBoundary(playerInfo->GetPos()))
		playerInfo->SetSpatialPartition(CSpatialPartitionManager::GetInstance()->UpdateGridInfo(playerInfo->GetPos()));

	//std::ostringstream ss2;
	//ss2 << "Player in Partition:" << playerInfo->GetSpatialPartition()->GetIndex();
	//if (playerInfo->CheckBoundary(playerInfo->GetPos()))
	//{
	//	playerInfo->ClearPartition();
	//	playerInfo->SetPartition(CSpatialPartitionManager::GetInstance()->GetPlayerGrid());
	//	playerInfo->SetMinBoundary(CSpatialPartitionManager::GetInstance()->GetPartition(playerInfo->GetPartition().front())->GetMinBoundary());
	//	playerInfo->SetMaxBoundary(CSpatialPartitionManager::GetInstance()->GetPartition(playerInfo->GetPartition().front())->GetMaxBoundary());
	//	CMasterEntityManager::GetInstance()->SetPartitionNum(playerInfo->GetPartition().front());
	//}

	std::ostringstream ss2;
	ss2 << "Player in Partition:" << playerInfo->GetSpatialPartition()->GetIndex();
	textObj[3]->SetText(ss2.str());

	//std::ostringstream ss3;
	//string toggle = CMasterEntityManager::GetInstance()->GetToggle() ? "ON" : "OFF";
	//ss3 << "Spatial Partition [Numpad 5]:" << toggle;
	//textObj[4]->SetText(ss3.str());
}

void SceneText::Render()
{
	RenderPassGPass();
	//currProg->UpdateInt("fogParam.enabled", 1);
	RenderPassMain();
	//currProg->UpdateInt("fogParam.enabled", 0);
}

void SceneText::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	playerInfo->DetachCamera();

	if (playerInfo->DropInstance() == false)
	{
#if _DEBUGMODE==1
		cout << "Unable to drop PlayerInfo class" << endl;
#endif
	}

	// Delete the lights
	delete lights[0];
	delete lights[1];
}

void SceneText::RenderPassGPass(void)
{
	DepthFBO::GetInstance()->m_renderPass = DepthFBO::RENDER_PASS_PRE;

	DepthFBO::GetInstance()->BindForWriting();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->SetActiveShader("gpass");

	if (lights[0]->type == Light::LIGHT_DIRECTIONAL)
	{
		DepthFBO::GetInstance()->m_lightDepthProj.SetToOrtho(-400, 400, -400, 400, -400, 800);
	}
	else
	{
		DepthFBO::GetInstance()->m_lightDepthProj.SetToPerspective(90, 1.f, 0.1, 20);
	}

	DepthFBO::GetInstance()->m_lightDepthView.SetToLookAt(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z, 0, 0, 0, 0, 1, 0);

	RenderWorld();
	EntityManager::GetInstance()->Render();
}

void SceneText::RenderPassMain(void)
{
	DepthFBO::GetInstance()->m_renderPass = DepthFBO::RENDER_PASS_MAIN;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, static_cast<GLsizei>(Application::GetInstance().GetWindowWidth()), static_cast<GLsizei>(Application::GetInstance().GetWindowHeight()));
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->SetActiveShader("default");

	//pass light depth texture 
	DepthFBO::GetInstance()->BindForReading(GL_TEXTURE8);
	currProg->UpdateInt("shadowMap", 8);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	EntityManager::GetInstance()->Render();
	RenderHelper::DrawLine(Vector3(10, 0, 0), Vector3(10, 10, 0), Color(1, 0, 0));
	RenderHelper::DrawLine(Vector3(-10, 0, 0), Vector3(10, 10, 0), Color(1, 0, 0));

	// Setup 2D pipeline then render 2D
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();
	EntityManager::GetInstance()->RenderUI();
}

void SceneText::RenderWorld(void)
{
}
