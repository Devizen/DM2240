#ifndef OPTION_H
#define OPTION_H

#include "ComponentBase.h"
#include "LuaInterface.h"
#include <iostream>

enum Type
{
	Audio = 0,
	Key
};

template<Type>
class OptionBase : public ComponentBase
{
public:
	OptionBase() {}
	~OptionBase() {}

	virtual void Init(void) {}
	virtual void Update(void) {}
	virtual void Render(void) {}
};

template <>
class OptionBase<Audio> : public ComponentBase
{
public:
	OptionBase() {}
	~OptionBase() {}

	virtual void Init(void) {}
	virtual void Update(void) {}
	virtual void Render(void) {}
};

template <>
class OptionBase<Key> : public ComponentBase
{
public:
	OptionBase() 
	{ 
		luaState = CLuaInterface::GetInstance()->luaState; 
		/*Saving key function.*/
		luaL_dofile(luaState, "Lua//SaveKey.lua");
	}
	~OptionBase() {}

	virtual void Init(void) {}
	virtual void Update(void) {}
	virtual void Update(const char* _keyIdentifier, char _updatedKey)
	{
		lua_getglobal(luaState, "SaveKey");
		char outputString[64];
		sprintf(outputString, "%s = %c\n", _keyIdentifier, _updatedKey);
		lua_pushstring(luaState, outputString);
		lua_pushinteger(luaState, false);
		lua_call(luaState, 2, 0);
	}
	virtual void Render(void) {}
private:
	lua_State* luaState;
};
#endif