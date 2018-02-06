#include "SceneManager.h"
#include "Scene.h"

#include "SceneText.h"
#include "Scene\CIntroScene.h"
#include "Scene\CMenuScene.h"
#include "Scene\CPauseScene.h"
#include "Scene\COptionScene.h"

#define PRINTINFO_HEADER "========== SceneStack Info =========="
#define PRINTINFO_FOOTER "======== End SceneStack Info ========"

SceneManager::SceneManager() :  nextScene(nullptr)//, activeScene(nullptr)
{
	sceneMap["Start"] = new SceneText();
	sceneMap["Intro"] = new CIntroScene();
	sceneMap["Menu"] = new CMenuScene();
	sceneMap["Pause"] = new CPauseScene();
	sceneMap["Option"] = new COptionScene();

	action = NONE;
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update(double _dt)
{
	// Check for change of scene
	switch (action)
	{
	case ACTION::PUSH:
		PushScene(nextScene);
		action = ACTION::NONE;
		break;
	case ACTION::POPNPUSH:
		PopScene();
		PushScene(nextScene);
		action = ACTION::NONE;
		break;
	case ACTION::POP:		
		PopScene();
		action = ACTION::NONE;
		break;
	case ACTION::MULTIPLE_POP:
		PopToScene(nextScene);
		action = ACTION::NONE;
		break;
	default:
		break;
	}

	//render the top of the stack only
	if (sceneStack.size())
		sceneStack.back()->Update(_dt);
	//for (std::deque<Scene*>::iterator it = sceneStack.begin(); it != sceneStack.end(); ++it)
	//{
	//	(*it)->Update(_dt);
	//}
	//if (activeScene)
	//	activeScene->Update(_dt);
}

void SceneManager::Render()
{
	for (std::deque<Scene*>::iterator it = sceneStack.begin(); it != sceneStack.end(); ++it)
	{
		(*it)->Render();
	}
	//if (activeScene)
	//	activeScene->Render();
}

void SceneManager::Exit()
{
	// Delete all scenes stored and empty the entire map
	for (s_Container::reverse_iterator it = sceneStack.rbegin(); it != sceneStack.rend(); ++it)
		(*it)->Exit();
	sceneStack.clear();

	std::map<std::string, Scene*>::iterator it, end;
	end = sceneMap.end();
	for (it = sceneMap.begin(); it != end; ++it)
	{
		delete it->second;
	}
	sceneMap.clear();
}

void SceneManager::AddScene(const std::string& _name, Scene* _scene)
{
	if (CheckSceneExist(_name))
	{
		// Scene Exist, unable to proceed
		throw std::exception("Duplicate scene name provided");
	}

	if (_scene == nullptr)
	{
		throw std::invalid_argument("Invalid scene pointer");
	}

	// Nothing wrong, add the scene to our map
	sceneMap[_name] = _scene;
}

void SceneManager::RemoveScene(const std::string& _name)
{
	// Does nothing if it does not exist
	if (!CheckSceneExist(_name))
		return;

	Scene* target = sceneMap[_name];
	//if (target == activeScene || target == nextScene)
	//{
	//	throw std::exception("Unable to remove active/next scene");
	//}

	// Delete and remove from our map
	delete target;
	sceneMap.erase(_name);
}

//void SceneManager::SetActiveScene(const std::string& _name)
//{
//	if (!CheckSceneExist(_name))
//	{
//		// Scene does not exist
//		throw std::exception("Scene does not exist");
//	}
//
//	// Scene exist, set the next scene pointer to that scene
//	nextScene = sceneMap[_name];
//}

bool SceneManager::CheckSceneExist(const std::string& _name)
{
	return sceneMap.count(_name) != 0;
}

void SceneManager::PushScene(const std::string & _name)
{
	if (!CheckSceneExist(_name)) // Scene does not exist
		throw std::exception("Scene does not exist");

	nextScene = sceneMap[_name];
	action = ACTION::PUSH;
}

void SceneManager::PopnPushScene(const std::string & _name)
{
	if (!CheckSceneExist(_name)) // Scene does not exist
		throw std::exception("Scene does not exist");

	nextScene = sceneMap[_name];
	action = ACTION::POPNPUSH;
}

void SceneManager::PopScene(const std::string & _name)
{
	if (!CheckSceneExist(_name)) // Scene does not exist
		throw std::exception("Scene does not exist");
	if (sceneStack.back() != sceneMap[_name])
		return;
	action = ACTION::POP;
}

void SceneManager::PopScene(Scene * scene)
{
	if (sceneStack.back() != scene)
		return;
	action = ACTION::POP;
}

void SceneManager::PopToScene(const std::string & _name)
{
	if (!CheckSceneExist(_name)) // Scene does not exist
		throw std::exception("Scene does not exist");
	Scene* targetScene = sceneMap[_name];
	for (s_Container::reverse_iterator it = sceneStack.rbegin(); it != sceneStack.rend(); ++it) {
		if (targetScene == *it)
		{
			nextScene = targetScene;
			action = ACTION::MULTIPLE_POP;
			return;
		}
	}
	throw std::exception("Scene does not exist in Stack");
}

void SceneManager::PrintSceneStackInfo()
{
	std::cout << PRINTINFO_HEADER << std::endl;
	std::cout << "Active Scenes: " << std::endl;
	for (s_Container::reverse_iterator it = sceneStack.rbegin(); it != sceneStack.rend(); ++it) { 
		for (std::map<std::string, Scene*>::iterator itMap = sceneMap.begin(); itMap != sceneMap.end(); ++itMap)
			if ((*itMap).second == *it)
				std::cout << (*itMap).first << std::endl;
	} 
	std::cout << PRINTINFO_FOOTER << std::endl;
}

void SceneManager::PushScene(Scene * next)
{
	Scene* scene = next;
	scene->Init();
	sceneStack.push_back(scene);
}

void SceneManager::PopScene()
{
	if (!sceneStack.empty())
	{
		Scene* prevScene = sceneStack.back();
		prevScene->Exit();
		sceneStack.pop_back();
	}
}

void SceneManager::PopToScene(Scene * next)
{
	//forces crash if next is not in stack
	while (next != sceneStack.back())
	{
		sceneStack.back()->Exit();
		sceneStack.pop_back();
	}
}
