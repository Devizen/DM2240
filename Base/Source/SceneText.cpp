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
#include "QuadTree\QuadTreeManager.h"

#include "Manager\CollisionManager.h"
#include "QuadTree\CameraManager.h"

/*Scene Graph.*/
#include "SceneGraph\SceneGraph.h"

/*For toggling AABB render.*/
#include "Enemy\EnemyManager.h"
#include "Enemy\Monk\Monk.h"

/*Create tower for enemy to attack.*/
#include "PlayerInfo\TowerManager.h"
#include "PlayerInfo\ScoreManager.h"
#include "ShaderProgram.h"

/*Lua Interface.*/
#include "LuaInterface.h"
/*Real-time Lua Script Editor.*/
#include "LuaEditor\LuaEditor.h"
/*Waypoint*/
#include "Waypoint\WaypointManager.h"

#include "Component\Option.h"
#include "MapEditor\MapEditor.h"

#include <iostream>
using namespace std;

//SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());

SceneText::SceneText()
{
}

//SceneText::SceneText(SceneManager* _sceneMgr)
//{
//	_sceneMgr->AddScene("Start", this);
//}

SceneText::~SceneText()
{
	//CSceneGraph::GetInstance()->Destroy();
}

void SceneText::Init()
{
	LuaEditor::GetInstance()->ReadLuaScript("Image//DM2240.lua");
	// White background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	//Load shader for shadow here. cause this is the only place where we need shadow
	m_gPassShaderID = GraphicsManager::GetInstance()->LoadShader("gpass", "Shader//GPass.vertexshader", "Shader//GPass.fragmentshader");

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

	//currProg->UpdateInt("numLights", 1);
	//currProg->UpdateInt("textEnabled", 0);

	ShaderProgram* currProg = Application::GetInstance().GetCurrProg();
	
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
	camera = new FPSCamera();
	camera->Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	playerInfo->AttachCamera(camera);
	GraphicsManager::GetInstance()->AttachCamera(camera);

	//MeshBuilder::GetInstance()->GenerateLine
	// Load all the meshes
	std::vector<std::string>object;
	std::vector<std::string>text;

	//TFunction* ObjectGeneration = CLuaInterface::GetInstance()->functionMap[CLuaInterface::FUNCTIONCALL::OBJECT];
	//dynamic_cast<FunctionPointerWrapper<void(std::vector<std::string>&, std::vector<std::string>)>*>(ObjectGeneration);
	//ObjectGeneration->Run(object, text);
	//std::deque<std::string>check = LuaEditor::GetInstance()->list;
	//CLuaInterface::GetInstance()->functionMap[CLuaInterface::OBJECT]->Run();

	MeshBuilder::GetInstance()->GetMesh("TEXT")->material.kAmbient.Set(1, 0, 0);
	MeshBuilder::GetInstance()->GetMesh("CONE")->material.kDiffuse.Set(0.99f, 0.99f, 0.99f);
	MeshBuilder::GetInstance()->GetMesh("CONE")->material.kSpecular.Set(0.f, 0.f, 0.f);
	
	/*Terrain.*/
	MeshBuilder::GetInstance()->GenerateTerrain("TERRAIN", "Image//TERRAIN//heightMap.raw", heightMap);
	MeshBuilder::GetInstance()->GetMesh("TERRAIN")->textureID[0] = LoadTGA("Image//WORLD//W_GRASS.tga");
	MeshBuilder::GetInstance()->GetMesh("TERRAIN")->textureID[1] = LoadTGA("Image//WORLD//W_SNOW.tga");
	MeshBuilder::GetInstance()->GetMesh("TERRAIN")->textureID[2] = LoadTGA("Image//WORLD//W_WATER.tga");

	/*Number of partitions for each X-axis and Z-axis.*/
	const unsigned numOfPartitionXZ = 5;
	groundScale = 400.f;

	/*Create spatial partitions with ground size.*/
	CSpatialPartitionManager::GetInstance()->Init(numOfPartitionXZ, static_cast<unsigned>(groundScale), playerInfo);

	playerInfo->SetSpatialPartition(CSpatialPartitionManager::GetInstance()->UpdateGridInfo(playerInfo->GetPos()));

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

	Create::Sprite2DObject("CROSSHAIR", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = (halfWindowWidth / halfWindowHeight) * 20.f;
	float halfFontSize = fontSize * 0.5f;
	for (int i = 0; i < 25; ++i)
	{
		textObj[i] = Create::Text2DObject("TEXT", Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 0.0f, 0.0f));
		textObj[i]->SetEntityType(ECEntityTypes::STATIC);
	}
	textObj[0]->SetText("Assignment 2");

	QuadTreeManager::GetInstance()->ground = groundEntity;
	//CameraManager::GetInstance()->AttachPlayerCam(&this->camera);
	CameraManager::GetInstance()->AttachPlayerCam(this->camera);

	FPSCamera* birdEyeCam = CameraManager::GetInstance()->GetBirdEyeCam();
	birdEyeCam->Init(Vector3(0, 500, 0), Vector3(0, 0, 0), Vector3(0, 0, -1));
	//birdEyeCam->

	/*Spawn randomly 5 enemies.*/
	//for (unsigned i = 0; i < 5; ++i) {
	//	Spawner("MONK");
	//}
	/*Audio Initialisation.*/
	audioPlayer = CAudioPlayer::GetInstance()->GetISoundEngine();
	audioPlayer->play2D("Audio/BGM/GAME.ogg", true);

	{

		//spawn monks
		//CEnemyManager::GetInstance()->SpawnMonk(Vector3(0, 0, 0));
	}

	int thisID = 
	CWaypointManager::GetInstance()->AddWaypoint(Vector3(-50, 0, -50));
	thisID= CWaypointManager::GetInstance()->AddWaypoint(thisID, Vector3(50, 0, -50));
	thisID = CWaypointManager::GetInstance()->AddWaypoint(thisID, Vector3(50, 0, 50));
	thisID = CWaypointManager::GetInstance()->AddWaypoint(thisID, Vector3(-50, 0, 50));

	MouseController::GetInstance()->SetKeepMouseCentered(true);
	Application::GetInstance().ShowMouse(false);
}

void SceneText::Update(double dt)
{
	static MapEditor* mapEditor = MapEditor::GetInstance();

	if (KeyboardController::GetInstance()->IsKeyPressed(VK_NUMPAD5))
		playerInfo->SetOptimiseUpdate(!*playerInfo->GetOptimiseUpdate());

	if (KeyboardController::GetInstance()->IsKeyPressed(VK_NUMPAD0))
	{
		mapEditor->SetEditorMode(!mapEditor->GetEditorMode());
	}
	//static OptionBase<Key>* test = new OptionBase<Key>();

	//if (KeyboardController::GetInstance()->IsKeyPressed(VK_NUMPAD0))
	//{
	//	test->Update("HELLO", 'a');
	//	std::cout << "SAVED" << std::endl;
	//}
	//if (KeyboardController::GetInstance()->IsKeyPressed(VK_NUMPAD5))
	//	LuaEditor::GetInstance()->SetIsToggle(!LuaEditor::GetInstance()->GetIsToggle());

	//if (KeyboardController::GetInstance()->IsKeyPressed('A'))
	//{
	//	(*LuaEditor::GetInstance()->GetLine()[2]) += 'A';
	//}

	if (KeyboardController::GetInstance()->IsKeyPressed('P'))
	{
		SceneManager::GetInstance()->PushScene("Pause");
	}

	if (KeyboardController::GetInstance()->IsKeyPressed('M'))
	{
		CameraManager::GetInstance()->toggleBirdEyeView = (CameraManager::GetInstance()->toggleBirdEyeView ? false : true);
	}
	if (KeyboardController::GetInstance()->IsKeyPressed(VK_HOME))
		SceneManager::GetInstance()->PrintSceneStackInfo();
	// <THERE>

	/*Randomly spawn an enemy on the map.*/
	if (KeyboardController::GetInstance()->IsKeyPressed(VK_ADD))
	{
		Create::Monk(Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
			Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(5.f, 5.f, 5.f), playerInfo);
	}

	/*Run for character.*/
	static bool activateDefaultSpeedUp = true;
	if (KeyboardController::GetInstance()->IsKeyDown(VK_LSHIFT)) {
		if (activateDefaultSpeedUp) {
			CameraManager::GetInstance()->speedUp = CameraManager::GetInstance()->defaultSpeedUp;
			activateDefaultSpeedUp = false;
		}
	}
	if (!activateDefaultSpeedUp)
		CameraManager::GetInstance()->speedUp += static_cast<float>(dt);
	if (KeyboardController::GetInstance()->IsKeyUp(VK_LSHIFT)) {
		activateDefaultSpeedUp = true;
		CameraManager::GetInstance()->speedUp = 1.f;
	}
	/*Reset the entire scene.*/
	if (KeyboardController::GetInstance()->IsKeyPressed('G')) {
		CScoreManager::GetInstance()->ResetScore();
		std::deque<EntityBase*>entityList = QuadTreeManager::GetInstance()->GetAllEntities();
		for (std::deque<EntityBase*>::iterator it = entityList.begin(); it != entityList.end(); ++it)
		{
			(*it)->SetIsDone(true);
		}
	}

	/*Display AABB of enemy.*/
	if (KeyboardController::GetInstance()->IsKeyPressed('K'))
	{
		CEnemyManager::GetInstance()->ToggleRenderAABB();
	}

	switch (mapEditor->GetEditorMode())
	{
		case false:
		{	
			static float horizontal = lights[0]->position.x;
			static float vertical = lights[0]->position.y;
			static float frontback = lights[0]->position.z;

			/*Spawner.*/
			//static float spawnTimer = 5.f;
			//static float spawnCooldown = 0.f;
			//spawnCooldown += static_cast<float>(dt);
			//if (spawnTimer <= 2.f)
			//	spawnTimer = 2.f;
			//if (spawnTimer - spawnCooldown < 0.f)
			//{
			//	std::string spawnType[] = { /*"MONK", */"TOWER", "CRATE", "BUILDING_1", "WALL" };
			//	Spawner(spawnType[Math::RandIntMinMax(0, /*4*/3)]);
			//	spawnCooldown = 0.f;
			//	spawnTimer -= 0.01f;
			//}

			/*Update the text object position and scale when the screen size is different. Example, Windowed Mode to Fullscreen Mode.*/
			float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
			float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
			float fontSize = (halfWindowWidth / halfWindowHeight) * 20.f;
			float halfFontSize = fontSize * 0.5f;
			for (size_t i = 0; i < sizeof(textObj) / sizeof(textObj[0]); ++i)
			{
				textObj[i]->SetPosition(Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f));
				textObj[i]->SetScale(Vector3(fontSize, fontSize, fontSize));
			}

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
			if (KeyboardController::GetInstance()->IsKeyDown('1'))
				glEnable(GL_CULL_FACE);
			if (KeyboardController::GetInstance()->IsKeyDown('2'))
				glDisable(GL_CULL_FACE);
			if (KeyboardController::GetInstance()->IsKeyDown('3'))
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if (KeyboardController::GetInstance()->IsKeyDown('4'))
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			if (KeyboardController::GetInstance()->IsKeyDown('5'))
			{
				lights[0]->type = Light::LIGHT_POINT;
			}
			else if (KeyboardController::GetInstance()->IsKeyDown('6'))
			{
				lights[0]->type = Light::LIGHT_DIRECTIONAL;
			}
			else if (KeyboardController::GetInstance()->IsKeyDown('7'))
			{
				lights[0]->type = Light::LIGHT_SPOT;
			}

			//if (KeyboardController::GetInstance()->IsKeyDown('I'))
			//	lights[0]->position.z -= (float)(10.f * dt);
			//if (KeyboardController::GetInstance()->IsKeyDown('K'))
			//	lights[0]->position.z += (float)(10.f * dt);
			//if (KeyboardController::GetInstance()->IsKeyDown('J'))
			//	lights[0]->position.x -= (float)(10.f * dt);
			//if (KeyboardController::GetInstance()->IsKeyDown('L'))
			//	lights[0]->position.x += (float)(10.f * dt);
			//if (KeyboardController::GetInstance()->IsKeyDown('O'))
			//	lights[0]->position.y -= (float)(10.f * dt);
			//if (KeyboardController::GetInstance()->IsKeyDown('P'))
			//	lights[0]->position.y += (float)(10.f * dt);

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

			if (KeyboardController::GetInstance()->IsKeyPressed(VK_RETURN))
			{
				QuadTreeManager::GetInstance()->toggle = (QuadTreeManager::GetInstance()->toggle ? false :
					CSpatialPartitionManager::GetInstance()->toggle = (CSpatialPartitionManager::GetInstance()->toggle ? false : true));
				//CSpatialPartitionManager::GetInstance()->toggle = (CSpatialPartitionManager::GetInstance()->toggle ? false : true);
				std::cout << "QuadTree " << QuadTreeManager::GetInstance()->toggle << "   SpatialPartition " << CSpatialPartitionManager::GetInstance()->toggle << std::endl;
			}


			// <THERE>

			/*Randomly spawn an enemy on the map.*/
			/*if (KeyboardController::GetInstance()->IsKeyDown(VK_BACK))
			{
				Create::Monk(Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
					Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(5.f, 5.f, 5.f), playerInfo);
			}*/

			/*Randomly spawn a tower on the map.*/
			if (KeyboardController::GetInstance()->IsKeyPressed('L'))
			{
				Create::Tower(Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
					Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(2.f, 2.f, 2.f));
			}

			/*Randomly spawn a crate on the map.*/
			if (KeyboardController::GetInstance()->IsKeyDown('I'))
			{
				GenericEntity* crate = Create::Asset("CRATE", Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
					Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(5.f, 5.f, 5.f), Vector3(5.f, 5.f, 5.f), true);
				crate->InitLoD("CRATE", "CRATE_MID", "CRATE_LOW");
				/*Create root for Scene Graph.*/
				CSceneNode* rootNode = Create::SceneNode(nullptr, nullptr, nullptr);
				rootNode->GetEntity()->SetPosition(crate->GetPosition());

				/*Create an empty Scene Graph which will be rendered and updated in QuadTreeManager.
				The entire Scene Graph will be traversed from the root.*/
				crate->SetSceneGraph(Create::SceneGraph(rootNode));
				Create::SceneNode(rootNode, rootNode, crate);

				/*Set AABB.*/
				crate->SetEntityType(ECEntityTypes::OBJECT);
				crate->collider = new CCollider(crate);
				crate->collider->SetMinAABB(Vector3(crate->GetPosition().x - 5.f, crate->GetPosition().y, crate->GetPosition().z - 5.f));
				crate->collider->SetMaxAABB(Vector3(crate->GetPosition().x + 5.f, crate->GetPosition().y + 10.f, crate->GetPosition().z + 5.f));
				crate->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(crate->GetPosition(), crate->GetScale()));
				CollisionManager::GetInstance()->AddCollider(crate->collider, crate->GetPartitionPtr());

				/*Set AABB.*/
				rootNode->GetEntity()->SetEntityType(ECEntityTypes::OBJECT);
				rootNode->GetEntity()->collider = new CCollider(rootNode->GetEntity());
				rootNode->GetEntity()->collider->SetMaxAABB(crate->collider->GetMaxAABB() - crate->GetPosition());
				rootNode->GetEntity()->collider->SetMinAABB(crate->collider->GetMinAABB() - crate->GetPosition());
				rootNode->GetEntity()->constMaxAABB = rootNode->GetEntity()->collider->GetMaxAABB();
				rootNode->GetEntity()->constMinAABB = rootNode->GetEntity()->collider->GetMinAABB();
				rootNode->GetEntity()->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(rootNode->GetEntity()->GetPosition(), rootNode->GetEntity()->GetScale()));
				CollisionManager::GetInstance()->AddCollider(rootNode->GetEntity()->collider, rootNode->GetEntity()->GetPartitionPtr());
			}

			/*Randomly spawn a Building_0 on the map.*/
			if (KeyboardController::GetInstance()->IsKeyDown('O'))
			{
				GenericEntity* building = Create::Asset("BUILDING_1", Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
					Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(0.2f, 0.2f, 0.2f), Vector3(5.f, 5.f, 5.f), true);
				building->InitLoD("BUILDING_1", "BUILDING_1_MID", "BUILDING_1_LOW");
				/*Create root for Scene Graph.*/
				CSceneNode* rootNode = Create::SceneNode(nullptr, nullptr, nullptr);
				rootNode->GetEntity()->SetPosition(building->GetPosition());

				/*Create an empty Scene Graph which will be rendered and updated in QuadTreeManager.
				The entire Scene Graph will be traversed from the root.*/
				building->SetSceneGraph(Create::SceneGraph(rootNode));
				Create::SceneNode(rootNode, rootNode, building);

				/*Set AABB.*/
				building->SetEntityType(ECEntityTypes::OBJECT);
				building->collider = new CCollider(building);
				building->collider->SetMinAABB(Vector3(building->GetPosition().x - 13.5f,
					building->GetPosition().y,
					building->GetPosition().z - 12.5f));
				building->collider->SetMaxAABB(Vector3(building->GetPosition().x + 13.5f,
					building->GetPosition().y + 45.f,
					building->GetPosition().z + 12.5f));
				building->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(building->GetPosition(), building->GetScale()));
				CollisionManager::GetInstance()->AddCollider(building->collider, building->GetPartitionPtr());

				/*Set AABB.*/
				rootNode->GetEntity()->SetEntityType(ECEntityTypes::OBJECT);
				rootNode->GetEntity()->collider = new CCollider(rootNode->GetEntity());
				rootNode->GetEntity()->collider->SetMaxAABB(building->collider->GetMaxAABB() - building->GetPosition());
				rootNode->GetEntity()->collider->SetMinAABB(building->collider->GetMinAABB() - building->GetPosition());
				rootNode->GetEntity()->constMaxAABB = rootNode->GetEntity()->collider->GetMaxAABB();
				rootNode->GetEntity()->constMinAABB = rootNode->GetEntity()->collider->GetMinAABB();
				rootNode->GetEntity()->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(rootNode->GetEntity()->GetPosition(), rootNode->GetEntity()->GetScale()));
				CollisionManager::GetInstance()->AddCollider(rootNode->GetEntity()->collider, rootNode->GetEntity()->GetPartitionPtr());
			}

			/*Randomly spawn a Wall on the map.*/
			if (KeyboardController::GetInstance()->IsKeyDown('U'))
			{
				GenericEntity* wall = Create::Asset("WALL", Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
					Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(5.f, 5.f, 5.f), Vector3(5.f, 5.f, 5.f), true);
				wall->InitLoD("WALL", "WALL_MID", "WALL_LOW");
				/*Create root for Scene Graph.*/
				CSceneNode* rootNode = Create::SceneNode(nullptr, nullptr, nullptr);
				rootNode->GetEntity()->SetPosition(wall->GetPosition());

				/*Create an empty Scene Graph which will be rendered and updated in QuadTreeManager.
				The entire Scene Graph will be traversed from the root.*/
				wall->SetSceneGraph(Create::SceneGraph(rootNode));
				Create::SceneNode(rootNode, rootNode, wall);

				/*Set AABB.*/
				wall->SetEntityType(ECEntityTypes::OBJECT);
				wall->collider = new CCollider(wall);
				/*Set AABB.*/
				wall->SetEntityType(ECEntityTypes::OBJECT);
				wall->collider = new CCollider(wall);
				wall->collider->SetMinAABB(Vector3(wall->GetPosition().x - 40.f,
					wall->GetPosition().y,
					wall->GetPosition().z - 0.5f));
				wall->collider->SetMaxAABB(Vector3(wall->GetPosition().x + 40.f,
					wall->GetPosition().y + 35.f,
					wall->GetPosition().z + 0.5f));
				wall->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(wall->GetPosition(), wall->GetScale()));
				wall->isDestroyable = true;
				CollisionManager::GetInstance()->AddCollider(wall->collider, wall->GetPartitionPtr());

				/*Set AABB.*/
				rootNode->GetEntity()->SetEntityType(ECEntityTypes::OBJECT);
				rootNode->GetEntity()->collider = new CCollider(rootNode->GetEntity());
				rootNode->GetEntity()->collider->SetMaxAABB(wall->collider->GetMaxAABB() - wall->GetPosition());
				rootNode->GetEntity()->collider->SetMinAABB(wall->collider->GetMinAABB() - wall->GetPosition());
				rootNode->GetEntity()->constMaxAABB = rootNode->GetEntity()->collider->GetMaxAABB();
				rootNode->GetEntity()->constMinAABB = rootNode->GetEntity()->collider->GetMinAABB();
				rootNode->GetEntity()->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(rootNode->GetEntity()->GetPosition(), rootNode->GetEntity()->GetScale()));
				CollisionManager::GetInstance()->AddCollider(rootNode->GetEntity()->collider, rootNode->GetEntity()->GetPartitionPtr());
			}
		break;
		}
		case true:
		{
			switch (mapEditor->status)
			{
				case MapEditor::PLACEOBJECT:
					if (MouseController::GetInstance()->IsButtonPressed(0))
					{
						Vector3 spawnPosition = (playerInfo->GetTarget() - playerInfo->GetPos()) * 50.f + camera->GetCameraPos();
						spawnPosition.y = -10.f;
						Create::Monk(spawnPosition, Vector3(5.f, 5.f, 5.f), playerInfo);
						std::cout << CEnemyManager::GetInstance()->GetEnemyList().size() << std::endl;
					}
					break;
				case MapEditor::PLACEWAYPOINT:
					break;
			}
			break;
		}
	}


	// Update the player position and other details based on keyboard and mouse inputs
	playerInfo->Update(dt);

	QuadTreeManager::GetInstance()->Update(dt);
	//Collision Updates
	CollisionManager::GetInstance()->Update(dt);
	QuadTreeManager::GetInstance()->PostUpdate(dt);


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

	if (CSpatialPartitionManager::GetInstance()->toggle && !QuadTreeManager::GetInstance()->toggle) {
		std::ostringstream ss2;
		ss2 << "Player in Partition:" << playerInfo->GetSpatialPartition()->GetIndex();
		textObj[3]->SetText(ss2.str());
	}
	else {
		ss1.str("");
		ss1 << "Entity Count:" << QuadTreeManager::GetInstance()->GetAllEntities().size();
		textObj[3]->SetText(ss1.str());
	}

	ss1.str("");
	ss1 << "Hitscan: " << (playerInfo->GetPrimaryWeapon()->hitScan ? "ON" : "OFF");
	textObj[4]->SetText(ss1.str());

	ss1.str("");
	ss1 << "Editor Mode: " << (mapEditor->GetEditorMode() ? "ON" : "OFF");
	textObj[5]->SetText(ss1.str());


	ss1.str("");
	ss1 << "Num Chase: " << CEnemyManager::GetInstance()->nearbyEnemies << " (" << CEnemyManager::GetInstance()->GetShouldAllChase() << ")";
	textObj[6]->SetText(ss1.str());

	ss1.str("");
	ss1 << "Editor Mode: " << (mapEditor->GetEditorMode() ? "ON" : "OFF");
	textObj[5]->SetText(ss1.str());

	ss1.str("");
	ss1 << "Score: " << std::to_string(static_cast<int>(CScoreManager::GetInstance()->GetScore()));
	textObj[21]->SetText(ss1.str());
	double aspectRatio = Application::GetInstance().GetWindowWidth() / (double)Application::GetInstance().GetWindowHeight();
	CameraManager::GetInstance()->SetAspect(aspectRatio);
	CScoreManager::GetInstance()->AddScore(dt);
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
	GraphicsManager::GetInstance()->RemoveLight(lights[0]->name);
	GraphicsManager::GetInstance()->RemoveLight(lights[1]->name);
	//delete lights[0];
	//delete lights[1];
	CScoreManager::GetInstance()->ResetScore();
	delete camera;
	CameraManager::GetInstance()->AttachPlayerCam(nullptr);
	//QuadTreeManager::GetInstance()->ground = nullptr;
	CWaypointManager::GetInstance()->ClearWaypoints();
	QuadTreeManager::GetInstance()->Clear();
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

	if (SceneManager::GetInstance()->IsSceneAtBottom(this))
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->SetActiveShader("default");

	//pass light depth texture 
	DepthFBO::GetInstance()->BindForReading(GL_TEXTURE8);
	ShaderProgram* currProg = Application::GetInstance().GetCurrProg();
	currProg->UpdateInt("shadowMap", 8);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(CameraManager::GetInstance()->GetFrustumFoV(),
		CameraManager::GetInstance()->GetFrustumAspect(), CameraManager::GetInstance()->GetNearPlane(), CameraManager::GetInstance()->GetFarPlane());
	//GraphicsManager::GetInstance()->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(CameraManager::GetInstance()->GetActiveCam());
	//EntityManager::GetInstance()->Render();

	if (!LuaEditor::GetInstance()->GetIsToggle())
	{
		CollisionManager::GetInstance()->RenderGrid();
	
		MS& ms = GraphicsManager::GetInstance()->GetModelStack();

		QuadTreeManager::GetInstance()->RenderGrid();
		CEnemyManager::GetInstance()->RenderStates();
		CWaypointManager::GetInstance()->RenderWaypoints();
		CameraManager::GetInstance()->RenderPlayerFrustum();

		static MapEditor* mapEditor = MapEditor::GetInstance();
		switch (mapEditor->GetEditorMode())
		{
		case true:
			static Mesh* monk[] = 
			{
				MeshBuilder::GetInstance()->GetMesh("MONK_HEAD"),
				MeshBuilder::GetInstance()->GetMesh("MONK_BODY"),
				MeshBuilder::GetInstance()->GetMesh("MONK_LEFT_ARM"),
				MeshBuilder::GetInstance()->GetMesh("MONK_RIGHT_ARM"),
				MeshBuilder::GetInstance()->GetMesh("MONK_LEFT_LEG"),
				MeshBuilder::GetInstance()->GetMesh("MONK_RIGHT_LEG")
			};

			static unsigned int tempSize = sizeof(monk) / sizeof(monk[0]);

			for (unsigned int i = 0; i < tempSize; ++i)
			{
				Vector3 offsetFromPlayer = (playerInfo->GetTarget() - playerInfo->GetPos()) * 50.f + camera->GetCameraPos();
				offsetFromPlayer.y = -10.f;
				MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
				modelStack.PushMatrix();
				modelStack.Translate(offsetFromPlayer);
				modelStack.Rotate(0.f, 0.f, 0.f, 1.f);
				modelStack.Scale(Vector3(5.f, 5.f, 5.f));
				RenderHelper::RenderMesh(monk[i]);
				modelStack.PopMatrix();
			}
			break;
		}
	}


	if (SceneManager::GetInstance()->IsSceneAtTop(this))
	{
		// Setup 2D pipeline then render 2D
		int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
		int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
		GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
		GraphicsManager::GetInstance()->DetachCamera();

		EntityManager::GetInstance()->RenderUI();
		//CScoreManager::GetInstance()->RenderUI();
		//std::cout << (*camera).GetCameraPos() << std::endl;

		//MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
		//modelStack.PushMatrix();
		//modelStack.Translate(0.f, 0.f, 0.f);
		//modelStack.Scale((halfWindowWidth / halfWindowHeight) * 50.f, (halfWindowWidth / halfWindowHeight) * 50.f, 1.f);
		//std::string editorModeStatus = MapEditor::GetInstance()->GetEditorMode() ? "Map Editor: ON" : "Map Editor: OFF";
		//RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), editorModeStatus, Color(1.f, 1.f, 0.f));
		//modelStack.PopMatrix();
	}



}

void SceneText::RenderWorld(void)
{
}

void SceneText::Spawner(std::string _name)
{
	if (_name == "MONK")
	{
		Create::Monk(Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
			Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(5.f, 5.f, 5.f), playerInfo);
	}

	/*Randomly spawn a tower on the map.*/
	if (_name == "TOWER")
	{
		Create::Tower(Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
			Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(2.f, 2.f, 2.f));
	}

	/*Randomly spawn a crate on the map.*/
	if (_name == "CRATE")
	{
		GenericEntity* crate = Create::Asset("CRATE", Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
			Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(5.f, 5.f, 5.f), Vector3(5.f, 5.f, 5.f), true);
		crate->InitLoD("CRATE", "CRATE_MID", "CRATE_LOW");
		/*Create root for Scene Graph.*/
		CSceneNode* rootNode = Create::SceneNode(nullptr, nullptr, nullptr);
		rootNode->GetEntity()->SetPosition(crate->GetPosition());

		/*Create an empty Scene Graph which will be rendered and updated in QuadTreeManager.
		The entire Scene Graph will be traversed from the root.*/
		crate->SetSceneGraph(Create::SceneGraph(rootNode));
		Create::SceneNode(rootNode, rootNode, crate);

		/*Set AABB.*/
		crate->SetEntityType(ECEntityTypes::OBJECT);
		crate->collider = new CCollider(crate);
		crate->collider->SetMinAABB(Vector3(crate->GetPosition().x - 5.f, crate->GetPosition().y, crate->GetPosition().z - 5.f));
		crate->collider->SetMaxAABB(Vector3(crate->GetPosition().x + 5.f, crate->GetPosition().y + 10.f, crate->GetPosition().z + 5.f));
		crate->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(crate->GetPosition(), crate->GetScale()));
		CollisionManager::GetInstance()->AddCollider(crate->collider, crate->GetPartitionPtr());

		/*Set AABB.*/
		rootNode->GetEntity()->SetEntityType(ECEntityTypes::OBJECT);
		rootNode->GetEntity()->collider = new CCollider(rootNode->GetEntity());
		rootNode->GetEntity()->collider->SetMaxAABB(crate->collider->GetMaxAABB() - crate->GetPosition());
		rootNode->GetEntity()->collider->SetMinAABB(crate->collider->GetMinAABB() - crate->GetPosition());
		rootNode->GetEntity()->constMaxAABB = rootNode->GetEntity()->collider->GetMaxAABB();
		rootNode->GetEntity()->constMinAABB = rootNode->GetEntity()->collider->GetMinAABB();
		rootNode->GetEntity()->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(rootNode->GetEntity()->GetPosition(), rootNode->GetEntity()->GetScale()));
		CollisionManager::GetInstance()->AddCollider(rootNode->GetEntity()->collider, rootNode->GetEntity()->GetPartitionPtr());
	}

	/*Randomly spawn a Building_0 on the map.*/
	if (_name == "BUILDING_1")
	{
		GenericEntity* building = Create::Asset("BUILDING_1", Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
			Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(0.2f, 0.2f, 0.2f), Vector3(5.f, 5.f, 5.f), true);
		building->InitLoD("BUILDING_1", "BUILDING_1_MID", "BUILDING_1_LOW");
		/*Create root for Scene Graph.*/
		CSceneNode* rootNode = Create::SceneNode(nullptr, nullptr, nullptr);
		rootNode->GetEntity()->SetPosition(building->GetPosition());

		/*Create an empty Scene Graph which will be rendered and updated in QuadTreeManager.
		The entire Scene Graph will be traversed from the root.*/
		building->SetSceneGraph(Create::SceneGraph(rootNode));
		Create::SceneNode(rootNode, rootNode, building);

		/*Set AABB.*/
		building->SetEntityType(ECEntityTypes::OBJECT);
		building->collider = new CCollider(building);
		building->collider->SetMinAABB(Vector3(building->GetPosition().x - 13.5f,
			building->GetPosition().y,
			building->GetPosition().z - 12.5f));
		building->collider->SetMaxAABB(Vector3(building->GetPosition().x + 13.5f,
			building->GetPosition().y + 45.f,
			building->GetPosition().z + 12.5f));
		building->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(building->GetPosition(), building->GetScale()));
		CollisionManager::GetInstance()->AddCollider(building->collider, building->GetPartitionPtr());

		/*Set AABB.*/
		rootNode->GetEntity()->SetEntityType(ECEntityTypes::OBJECT);
		rootNode->GetEntity()->collider = new CCollider(rootNode->GetEntity());
		rootNode->GetEntity()->collider->SetMaxAABB(building->collider->GetMaxAABB() - building->GetPosition());
		rootNode->GetEntity()->collider->SetMinAABB(building->collider->GetMinAABB() - building->GetPosition());
		rootNode->GetEntity()->constMaxAABB = rootNode->GetEntity()->collider->GetMaxAABB();
		rootNode->GetEntity()->constMinAABB = rootNode->GetEntity()->collider->GetMinAABB();
		rootNode->GetEntity()->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(rootNode->GetEntity()->GetPosition(), rootNode->GetEntity()->GetScale()));
		CollisionManager::GetInstance()->AddCollider(rootNode->GetEntity()->collider, rootNode->GetEntity()->GetPartitionPtr());
	}

	/*Randomly spawn a Wall on the map.*/
	if (_name == "WALL")
	{
		GenericEntity* wall = Create::Asset("WALL", Vector3(Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f), -10.f,
			Math::RandFloatMinMax(-(groundScale * 0.5f) + 1.f, (groundScale * 0.5f) - 1.f)), Vector3(5.f, 5.f, 5.f), Vector3(5.f, 5.f, 5.f), true);
		wall->InitLoD("WALL", "WALL_MID", "WALL_LOW");
		/*Create root for Scene Graph.*/
		CSceneNode* rootNode = Create::SceneNode(nullptr, nullptr, nullptr);
		rootNode->GetEntity()->SetPosition(wall->GetPosition());

		/*Create an empty Scene Graph which will be rendered and updated in QuadTreeManager.
		The entire Scene Graph will be traversed from the root.*/
		wall->SetSceneGraph(Create::SceneGraph(rootNode));
		Create::SceneNode(rootNode, rootNode, wall);

		/*Set AABB.*/
		wall->SetEntityType(ECEntityTypes::OBJECT);
		wall->collider = new CCollider(wall);
		/*Set AABB.*/
		wall->SetEntityType(ECEntityTypes::OBJECT);
		wall->collider = new CCollider(wall);
		wall->collider->SetMinAABB(Vector3(wall->GetPosition().x - 40.f,
			wall->GetPosition().y,
			wall->GetPosition().z - 0.5f));
		wall->collider->SetMaxAABB(Vector3(wall->GetPosition().x + 40.f,
			wall->GetPosition().y + 35.f,
			wall->GetPosition().z + 0.5f));
		wall->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(wall->GetPosition(), wall->GetScale()));
		wall->isDestroyable = true;
		CollisionManager::GetInstance()->AddCollider(wall->collider, wall->GetPartitionPtr());

		/*Set AABB.*/
		rootNode->GetEntity()->SetEntityType(ECEntityTypes::OBJECT);
		rootNode->GetEntity()->collider = new CCollider(rootNode->GetEntity());
		rootNode->GetEntity()->collider->SetMaxAABB(wall->collider->GetMaxAABB() - wall->GetPosition());
		rootNode->GetEntity()->collider->SetMinAABB(wall->collider->GetMinAABB() - wall->GetPosition());
		rootNode->GetEntity()->constMaxAABB = rootNode->GetEntity()->collider->GetMaxAABB();
		rootNode->GetEntity()->constMinAABB = rootNode->GetEntity()->collider->GetMinAABB();
		rootNode->GetEntity()->SetPartition(CSpatialPartitionManager::GetInstance()->GetPartitionIndices(rootNode->GetEntity()->GetPosition(), rootNode->GetEntity()->GetScale()));
		CollisionManager::GetInstance()->AddCollider(rootNode->GetEntity()->collider, rootNode->GetEntity()->GetPartitionPtr());
	}
}
