#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "SingletonTemplate.h"
#include <map>
#include <string>
#include <deque>

class Scene;

class SceneManager : public Singleton<SceneManager>
{
	friend Singleton<SceneManager>;
public:
	// System Interface
	void Update(double _dt);
	void Render();
	void Exit();

	// User Interface
	void AddScene(const std::string& _name, Scene* _scene);
	void RemoveScene(const std::string& _name);
	//void SetActiveScene(const std::string& _name);
	bool CheckSceneExist(const std::string& _name);

	//Init the scene then add to stack to update simultaneously
	//UI
	void PushScene(const std::string& _name);
	void PopnPushScene(const std::string& _name);
	void PopScene(const std::string& _name);
	void PopScene(Scene* scene);
	void PopToScene(const std::string& _name);

	void PrintSceneStackInfo();

	bool IsSceneAtBottom(Scene* scene);
	bool IsSceneAtTop(Scene* scene) { if (sceneStack.empty()) return false; return (sceneStack.back() == scene); }
	std::string GetScene(int decre = 0) { if (decre > 0) return ""; if (-decre >= sceneStack.size()) return GetSceneName(sceneStack.front()); return GetSceneName(sceneStack.at(sceneStack.size() + decre - 1)); }

	enum MESSAGE
	{
		NOMESSAGE = 0,
		STARTRENDER,
		STOPRENDER,
		TOTAL
	};
	void PushMessage(std::string _sceneName, MESSAGE _type);

private:
	SceneManager();
	~SceneManager();

	std::map<std::string, Scene*> sceneMap;
	std::string currScene;
	//Scene* activeScene, *nextScene, *previousScene;
	Scene* nextScene;
	typedef std::deque<Scene*> s_Container;
	s_Container sceneStack;

	enum ACTION
	{
		NONE,
		PUSH,
		POPNPUSH,
		POP,
		MULTIPLE_POP
	} action;

	std::deque<std::pair<std::string, MESSAGE>>messageDeque;

	void PushScene(Scene* next);
	void PopScene();
	void PopToScene(Scene* next);

	void UpdateMessage(double _dt);

	std::string GetSceneName(Scene* scene) {
		for (auto& pair : sceneMap) if (pair.second == scene) return pair.first; return "";
	}
};

#endif // SCENE_MANAGER_H