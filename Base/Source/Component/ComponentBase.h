#ifndef COMPONENTBASE_H
#define COMPONENTBASE_H

class ComponentBase
{
public:
	//ComponentBase() {};
	virtual ~ComponentBase() {};
	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Render(void) = 0;
};
#endif
