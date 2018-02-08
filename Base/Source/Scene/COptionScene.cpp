#include "COptionScene.h"

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

struct InputKeyConfig : public UIObj
{
	TextField* keyname;
	TextField* keyinput;
	Button* keyinputbutton;
};

COptionScene::COptionScene()
{
}

COptionScene::~COptionScene()
{
}

void COptionScene::Init()
{
	MouseController::GetInstance()->SetKeepMouseCentered(false);
	Application::GetInstance().ShowMouse(true);
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	MeshBuilder::GetInstance()->GenerateQuad("OPTION_BACKGROUND", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("OPTION_BACKGROUND")->textureID[0] = LoadTGA("Image//SKYPLANE.tga");
	MeshBuilder::GetInstance()->GenerateQuad("OPTION_CLOSE_BUTTON", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("OPTION_CLOSE_BUTTON")->textureID[0] = LoadTGA("Image//UI//closebutton.tga");
	MeshBuilder::GetInstance()->GenerateQuad("OPTION_OPTION_BUTTON", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("OPTION_OPTION_BUTTON")->textureID[0] = LoadTGA("Image//UI//optionbutton.tga");
	MeshBuilder::GetInstance()->GenerateQuad("TEXTFIELD_BG", Color(1, 1, 1), 1.f);
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;

	Button* startButton = new Button("Button");
	startButton->set_scale_x(30).set_scale_y(30).set_x(halfWindowWidth * 0.8f).set_y(halfWindowHeight * 0.8f);
	startButton->image = MeshBuilder::GetInstance()->GetMesh("OPTION_CLOSE_BUTTON");
	uiObjList.push_back(startButton);

	Button* optionButton = new Button("Option");
	optionButton->set_scale_x(50).set_scale_y(50).set_x(0).set_y(0);
	optionButton->image = MeshBuilder::GetInstance()->GetMesh("OPTION_OPTION_BUTTON");
	uiObjList.push_back(optionButton);

	float aspectXRatio = Application::GetInstance().GetWindowWidth() / 800.f;
	float aspectYRatio = Application::GetInstance().GetWindowHeight() / 600.f;
	float textSize = 15.f;
	Mesh* textfieldbg = MeshBuilder::GetInstance()->GetMesh("TEXTFIELD_BG");
	float textfieldscaleX = 20;
	float textfieldscaleY= 15;


	InputKeyConfig* ikc = new InputKeyConfig();
	TextField* textField = new TextField("textfield", textfieldbg, "Move Forward", Color(0, 0, 0), textSize);
	textField->set_scale_x(textfieldscaleX * aspectXRatio).set_scale_y(textfieldscaleY * aspectYRatio).set_x(-300).set_y(0);
	TextField* textField2 = new TextField("textfield2", textfieldbg, "Char", Color(0, 0, 0), textSize);
	textField2->set_scale_x(textfieldscaleX * aspectXRatio).set_scale_y(textfieldscaleY * aspectYRatio).set_x(200).set_y(0);
	Button* button = new Button("button");
	button->set_scale_x(textfieldscaleX * aspectXRatio).set_scale_y(textfieldscaleY * aspectYRatio).set_x(200).set_y(0);
	ikc->keyname = textField;
	ikc->keyinput = textField2;
	ikc->keyinputbutton = button;

	uiObjList.push_back(ikc);
}

void COptionScene::Update(double dt)
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
						std::cout << "Closing Option" << std::endl;
						SceneManager::GetInstance()->PopScene(this);
					}
				}


			}
		}

	}

	if (KeyboardController::GetInstance()->IsKeyPressed(VK_HOME))
		SceneManager::GetInstance()->PrintSceneStackInfo();
}

void COptionScene::Render()
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
	RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("OPTION_BACKGROUND"));
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
		TextField* tf = dynamic_cast<TextField*>(*it);
		if (tf)
		{
			ms.PushMatrix();
			ms.Translate(tf->x, tf->y, 2);
			ms.Scale(tf->scale_x, tf->scale_y, 1);
			RenderHelper::RenderMesh(tf->background);
			ms.PopMatrix();

			ms.PushMatrix();
			ms.Translate(tf->x, tf->y, 2);
			ms.Scale(tf->textScale, tf->textScale, 1);
			RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), tf->text, tf->textColor);
			ms.PopMatrix();
		}
		InputKeyConfig* ikc = dynamic_cast<InputKeyConfig*>(*it);
		if (ikc)
		{
			ms.PushMatrix();
			ms.Translate(ikc->x, ikc->y, 1);

			TextField* tField = static_cast<TextField*>(ikc->keyname);
			ms.PushMatrix();
			ms.Translate(tField->x, tField->y, 1);
			ms.Scale(tField->scale_x, tField->scale_y, 1);
			RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), tField->text, tField->textColor);
			ms.PopMatrix();

			tField = static_cast<TextField*>(ikc->keyinput);
			ms.PushMatrix();
			ms.Translate(tField->x, tField->y, 1);
			ms.Scale(tField->scale_x, tField->scale_y, 1);
			RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), tField->text, tField->textColor);
			ms.PopMatrix();

			//Button* button = static_cast<Button*>(ikc->keyinputbutton);
			//ms.PushMatrix();
			//ms.Translate(button->x, button->y, 1);
			//ms.Scale(tField->scale_x, tField->scale_y, 1);
			//RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), tf->text, tf->textColor);
			//ms.PopMatrix();

			ms.PopMatrix();
		}
	}
}

void COptionScene::Exit()
{
	MeshBuilder::GetInstance()->RemoveMesh("OPTION_BACKGROUND");
	MeshBuilder::GetInstance()->RemoveMesh("OPTION_CLOSE_BUTTON");
	MeshBuilder::GetInstance()->RemoveMesh("OPTION_OPTION_BUTTON");
	MeshBuilder::GetInstance()->RemoveMesh("TEXTFIELD_BG");

	for (auto & uiobj : uiObjList)
		delete uiobj;
	uiObjList.clear();
}
