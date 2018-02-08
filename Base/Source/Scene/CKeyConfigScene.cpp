#include "CKeyConfigScene.h"

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

CKeyConfigScene::CKeyConfigScene()
{
}

CKeyConfigScene::~CKeyConfigScene()
{
}

void CKeyConfigScene::Init()
{
	MouseController::GetInstance()->SetKeepMouseCentered(false);
	Application::GetInstance().ShowMouse(true);
	camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	MeshBuilder::GetInstance()->GenerateQuad("KC_OPTION_BACKGROUND", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("KC_OPTION_BACKGROUND")->textureID[0] = LoadTGA("Image//UI//pausequad.tga");
	MeshBuilder::GetInstance()->GenerateQuad("KC_OPTION_CLOSE_BUTTON", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("KC_OPTION_CLOSE_BUTTON")->textureID[0] = LoadTGA("Image//UI//closebutton.tga");
	//MeshBuilder::GetInstance()->GenerateQuad("KC_OPTION_OPTION_BUTTON", Color(1, 1, 1), 1.f);
	//MeshBuilder::GetInstance()->GetMesh("KC_OPTION_OPTION_BUTTON")->textureID[0] = LoadTGA("Image//UI//optionbutton.tga");
	MeshBuilder::GetInstance()->GenerateQuad("KC_TEXTFIELD_BG", Color(1, 1, 1), 1.f);
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() * 0.5f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() * 0.5f;

	Button* startButton = new Button("Button");
	startButton->set_scale_x(30).set_scale_y(30).set_x(halfWindowWidth * 0.8f).set_y(halfWindowHeight * 0.8f);
	startButton->image = MeshBuilder::GetInstance()->GetMesh("KC_OPTION_CLOSE_BUTTON");
	uiObjList.push_back(startButton);

	float aspectXRatio = Application::GetInstance().GetWindowWidth() / 800.f;
	float aspectYRatio = Application::GetInstance().GetWindowHeight() / 600.f;
	float textSize = 15.f;
	Mesh* textfieldbg = MeshBuilder::GetInstance()->GetMesh("KC_TEXTFIELD_BG");
	float textfieldscaleX = 20;
	float textfieldscaleY = 15;


	saveFile = new OptionBase<Key>();
	//saveFile->Remove();
	int index = 0;
	for (std::map<int*, std::pair<std::string, std::function<void(float)>>>::iterator
		it = CPlayerInfo::GetInstance()->GetBindKeyMap().begin(); it != CPlayerInfo::GetInstance()->GetBindKeyMap().end(); ++it)
	{
		//saveFile->Update(it->second.first.c_str(), *it->first);
		float the_y = 230 - index * 50;
		if ((*it).second.first == "")
			break;

		InputKeyConfig* ikc = new InputKeyConfig();
		ikc->set_y(the_y);

		TextField* textField = new TextField("textfield", textfieldbg, (*it).second.first, Color(0, 0, 0), textSize);
		textField->set_scale_x(textfieldscaleX * aspectXRatio).set_scale_y(textfieldscaleY * aspectYRatio).set_x(-300).set_y(0);
		std::string text;
		TextField* textField2 = new TextField("textfield2", textfieldbg, std::to_string((*(*it).first)), Color(0, 0, 0), textSize);

		textField2->asciiText = *it->first;

		textField2->set_scale_x(textfieldscaleX * aspectXRatio).set_scale_y(textfieldscaleY * aspectYRatio).set_x(200).set_y(0);
		Button* button = new Button("button");
		button->set_scale_x(textfieldscaleX * aspectXRatio).set_scale_y(textfieldscaleY * aspectYRatio).set_x(200).set_y(the_y);
		button->image = textfieldbg;
		ikc->keyname = textField;
		ikc->keyinput = textField2;
		ikc->keyinputbutton = button;

		ikc->input = it->first;

		uiObjList.push_back(ikc);
		++index;
	}
}

void CKeyConfigScene::Update(double dt)
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
						SceneManager::GetInstance()->PushMessage("Option", SceneManager::MESSAGE::STARTRENDER);
					}
				}
			}
		}
		InputKeyConfig* ikc = dynamic_cast<InputKeyConfig*>(*it);
		if (ikc)
		{
			if (ikc->keyinputbutton->CheckCollision(cursor_x, cursor_y))
			{
				if (MouseController::GetInstance()->IsButtonPressed(0) && selectedConfig == nullptr)
				{
					selectedConfig = ikc;
					std::cout << "wad" << std::endl;
					selectButtonReleased = false;
				}
			}
		}

	}


	if (selectedConfig && selectButtonReleased)
	{
		/*Wait for player to input another key to overwrite the current key.*/
		if (!KeyboardController::GetInstance()->GetKeyInput()->empty())
		{
#ifdef _DEBUG
			std::cout << "I AM PRESSSING: " << KeyboardController::GetInstance()->GetKeyInput()->front() << std::endl;
#endif
			*selectedConfig->input = KeyboardController::GetInstance()->GetKeyInput()->front();
			saveFile->Remove();
			auto bindKeyMap = CPlayerInfo::GetInstance()->GetBindKeyMap();

			for (std::map<int*, std::pair<std::string, std::function<void(float)>>>::iterator it = bindKeyMap.begin(); it != bindKeyMap.end(); ++it)
			{
				saveFile->Update(it->second.first.c_str(), *it->first);
			}
			saveFile->Complete();
			char temp[64];
			sprintf(temp, "%c", KeyboardController::GetInstance()->GetKeyInput()->front());
			selectedConfig->keyinput->text = std::to_string(KeyboardController::GetInstance()->GetKeyInput()->front());
			selectedConfig->keyinput->asciiText = KeyboardController::GetInstance()->GetKeyInput()->front();
			selectedConfig = nullptr;

			std::cout << "bye" << std::endl;
		}
	}
	if (MouseController::GetInstance()->IsButtonUp(0))
	{
		selectButtonReleased = true;
		//std::cout << "up" << std::endl;
		while (KeyboardController::GetInstance()->GetKeyInput()->size())
		{
			/*Clearing the key input.*/
			KeyboardController::GetInstance()->GetKeyInput()->pop();
#ifdef _DEBUG
			std::cout << "Current size in queue: " <<
				KeyboardController::GetInstance()->GetKeyInput()->size() << std::endl;
#endif
		}
	}

}

void CKeyConfigScene::Render()
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
	RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("KC_OPTION_BACKGROUND"));
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

			ms.PushMatrix();
			ms.Translate(tField->x, tField->y, 1);
			ms.Scale(tField->scale_x, tField->scale_y, 1);
			RenderHelper::RenderText(MeshBuilder::GetInstance()->GetMesh("TEXT"), text, tField->textColor);
			ms.PopMatrix();


			ms.PopMatrix();
		}
	}
}

void CKeyConfigScene::Exit()
{
	MeshBuilder::GetInstance()->RemoveMesh("KC_OPTION_BACKGROUND");
	MeshBuilder::GetInstance()->RemoveMesh("KC_OPTION_CLOSE_BUTTON");
	MeshBuilder::GetInstance()->RemoveMesh("KC_OPTION_OPTION_BUTTON");
	MeshBuilder::GetInstance()->RemoveMesh("KC_TEXTFIELD_BG");

	for (auto & uiobj : uiObjList)
		delete uiobj;
	uiObjList.clear();
}
