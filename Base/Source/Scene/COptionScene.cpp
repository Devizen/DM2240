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
#include "../PlayerInfo/PlayerInfo.h"

struct InputKeyConfig : public UIObj
{
	TextField* keyname;
	TextField* keyinput;
	Button* keyinputbutton;
	int* input;
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

	Button* keyConfigButton = new Button("keyConfig");
	keyConfigButton->set_scale_x(50).set_scale_y(50).set_x(0).set_y(100);
	keyConfigButton->image = MeshBuilder::GetInstance()->GetMesh("OPTION_OPTION_BUTTON");
	uiObjList.push_back(keyConfigButton);

	Button* audioConfigButton = new Button("audioConfig");
	audioConfigButton->set_scale_x(50).set_scale_y(50).set_x(0).set_y(0);
	audioConfigButton->image = MeshBuilder::GetInstance()->GetMesh("OPTION_OPTION_BUTTON");
	uiObjList.push_back(audioConfigButton);

	Button* videoConfigButton = new Button("videoConfig");
	videoConfigButton->set_scale_x(50).set_scale_y(50).set_x(0).set_y(-100);
	videoConfigButton->image = MeshBuilder::GetInstance()->GetMesh("OPTION_OPTION_BUTTON");
	uiObjList.push_back(videoConfigButton);

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

	static InputKeyConfig* selectedConfig = nullptr;
	static bool selectButtonReleased = true;

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
						SceneManager::GetInstance()->PushMessage(SceneManager::GetInstance()->GetScene(-1), SceneManager::MESSAGE::STARTRENDER);
					}
					if (button->name == "keyConfig")
					{
						std::cout << "Opening Key Config" << std::endl;
						SceneManager::GetInstance()->PushScene("KeyConfig");
						SceneManager::GetInstance()->PushMessage("Option", SceneManager::MESSAGE::STOPRENDER);
					}
					if (button->name == "audioConfig")
					{
						std::cout << "Opening Audio Config" << std::endl;
						SceneManager::GetInstance()->PushScene("AudioConfig");
						SceneManager::GetInstance()->PushMessage("Option", SceneManager::MESSAGE::STOPRENDER);
					}
					if (button->name == "videoConfig")
					{
						std::cout << "Opening Video Config" << std::endl;
						SceneManager::GetInstance()->PushScene("VideoConfig");
						SceneManager::GetInstance()->PushMessage("Option", SceneManager::MESSAGE::STOPRENDER);
					}
				}
			}
		}

	}

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

			ms.PopMatrix();

			Button* button = static_cast<Button*>(ikc->keyinputbutton);
			ms.PushMatrix();
			ms.Translate(button->x, button->y, 1);
			ms.Scale(button->scale_x, button->scale_y, 1);
			RenderHelper::RenderMesh(button->image);
			ms.PopMatrix();

			ms.PushMatrix();
			ms.Translate(ikc->x, ikc->y, 1);

			tField = static_cast<TextField*>(ikc->keyinput);

			std::string text = "";

			switch (tField->asciiText)
			{
			case 0:
				text = "LMB";
				break;
			case 1:
				text = "RMB";
				break;
			case 32:
				text = "SPC";
				break;
			case 262:
				text = "RIGHT";
				break;
			case 263:
				text = "LEFT";
				break;
			case 264:
				text = "DOWN";
				break;
			case 265:
				text = "UP";
				break;
			default:
				char temp[64];
				sprintf(temp, "%c", *ikc->input);
				text = temp;
				break;
			}

			////std::string text = tField->text;
			//if (text == "0")
			//	text = "LMB";
			//else if (text == "1")
			//	text = "RMB";
			//else
			//{
			//	text = (char)std::atoi(tField->text.c_str());
			//	if (text == " ")
			//		text = "SPACE";
			//}

			ms.PushMatrix();
			ms.Translate(tField->x, tField->y, 1);
			ms.Scale(tField->scale_x, tField->scale_y, 1);
			RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), text, tField->textColor);
			ms.PopMatrix();


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
