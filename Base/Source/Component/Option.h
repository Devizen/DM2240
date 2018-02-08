#ifndef OPTION_H
#define OPTION_H

#include "ComponentBase.h"
#include "LuaInterface.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

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
	virtual void Update(const char* _keyIdentifier, int _updatedKey, std::string _savePath = "Lua//tempKeys.lua")
	{
		lua_getglobal(luaState, "SaveKey");
		char outputString[64];
		sprintf(outputString, "%s = %d\n", _keyIdentifier, _updatedKey);
		lua_pushstring(luaState, _savePath.c_str());
		lua_pushstring(luaState, outputString);
		lua_pushinteger(luaState, false);
		lua_call(luaState, 3, 0);
	}
	virtual void Complete(std::string _tempPath = "Lua//tempKeys.lua", std::string _savePath = "Lua//Keyboard.lua")
	{
		std::string header = "\n<START REMOVING PREVIOUS KEYBOARD FILE FROM: " + _savePath + '>';
		std::string footer = "<END REMOVING PREVIOUS KEYBOARD FILE FROM: " + _savePath + ">\n";
		std::string rename = "<Successfully renamed " + _tempPath + " to " + _savePath + ".>";
		std::string failRename = "<Failed to rename " + _tempPath + " to " + _savePath + ".>";
		struct stat buffer;

#ifdef _DEBUG
		std::cout << header << std::endl;
#endif
		if (stat(_savePath.c_str(), &buffer) == 0)
		{
#ifdef _DEBUG
			std::cout << "<File name: " << _savePath << " exists.>" << std::endl;
			std::cout << "<Removing " << _savePath << ">" << std::endl;
#endif
			remove(_savePath.c_str());
		}
		int result = std::rename(_tempPath.c_str(), _savePath.c_str());
		
#ifdef _DEBUG
		switch (result)
		{
			/*Success.*/
		case 0:
			std::cout << rename << std::endl;
			break;
			/*Failed.*/
		case 1:
			std::cout << failRename << std::endl;
			break;
		}
#endif
	}
	virtual void Remove(std::string _path = "Lua//tempKeys.lua")
	{
		std::string header = "\n<START REMOVING TEMPORARY FILE FROM: " + _path + '>';
		std::string footer = "<END REMOVING TEMPORARY FILE FROM: " + _path + ">\n";

#ifdef _DEBUG
		std::cout << header << std::endl;
#endif
		struct stat buffer;
		if (stat(_path.c_str(), &buffer) == 0)
		{
#ifdef _DEBUG
			std::cout << "<File name: " << _path << " exists.>" << std::endl;
			std::cout << "<Removing " << _path << ">" << std::endl;
			std::cout << footer << std::endl;
#endif
			remove(_path.c_str());
		}
#ifdef _DEBUG
		else
		{
			std::cout << "<File name: " << _path << " does not exists.>" << std::endl;
			std::cout << footer << std::endl;
		}
#endif
	}
	virtual void Render(void) {}
private:
	lua_State* luaState;
};
#endif