#ifndef SCENE_H
#define SCENE_H

class Scene
{
public:
	Scene() : stopRender(false) {}
	virtual ~Scene() {}

	virtual void Init() = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;
	bool stopRender;
};

#endif // SCENE_H