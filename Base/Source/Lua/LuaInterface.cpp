#include "LuaInterface.h"
#include <iostream>
#include <vector>
CLuaInterface* CLuaInterface::s_instance = 0;

CLuaInterface::~CLuaInterface()
{
}

CLuaInterface * CLuaInterface::GetInstance()
{
	if (s_instance == nullptr)
	{
		s_instance = new CLuaInterface();
		for (int i = 0; i < static_cast<int>(FUNCTIONCALL::TOTAL); ++i)
		{
			if (i == static_cast<int>(FUNCTIONCALL::INIT))
			{
				std::cout << "<Attemping to Initialise Lua State.>" << std::endl;
				s_instance->isInit = false;
				/*Create Init function.*/
				s_instance->functionMap.insert(std::make_pair(FUNCTIONCALL::INIT, []()->void
				{
					/*Create Lua state.*/
					s_instance->luaState = lua_open();

					if (s_instance->luaState)
					{
						/*Load Lua auxiliary libraries.*/
						luaL_openlibs(s_instance->luaState);

						luaL_dofile(s_instance->luaState, "Image//DM2240.lua");
						s_instance->isInit = true;
						std::cout << "<Initialised Lua State from "<< "Image//DM2240.lua>" << std::endl;
					}
					if (!s_instance->isInit)
						std::cout << "<Failed to initialise Lua State.>" << std::endl;
				}));
			}
			else if (i == static_cast<int>(FUNCTIONCALL::RUN))
			{
				s_instance->isInit = false;
				/*Create Run function.*/
				s_instance->functionMap.insert(std::make_pair(FUNCTIONCALL::RUN, []()->void 
				{
					/*Return if the luaState is not initialised.*/
					if (s_instance->luaState == nullptr)
						return;

					/*Read variables.*/
					/*Go into .lua to find the "title".*/
					lua_getglobal(s_instance->luaState, "title");

					/*Extract value, -1 because we extract 1 value from lua_getglobal.*/
					const char* title = lua_tostring(s_instance->luaState, -1);

					lua_getglobal(s_instance->luaState, "width");
					int width = lua_tointeger(s_instance->luaState, -1);

					lua_getglobal(s_instance->luaState, "height");
					int height = lua_tointeger(s_instance->luaState, -1);

					/*Display the values on screen.*/
					std::cout << title << std::endl;
					std::cout << "--------------------------------------" << std::endl;
					std::cout << "Width of Screen: " << width << std::endl;
					std::cout << "Height of Screen: " << height << std::endl;

				}));
			}
			else if (i == static_cast<int>(FUNCTIONCALL::DROP))
			{
				s_instance->isInit = false;
				/*Create Run function.*/
				s_instance->functionMap.insert(std::make_pair(FUNCTIONCALL::DROP, []()->void 
				{
					if (s_instance->luaState)
						lua_close(s_instance->luaState);
				}));
			}
		}
	}
	return s_instance;
}

bool CLuaInterface::DropInstance()
{
	if (s_instance)
	{
		/*Drop the Lua Interface Class.*/
		s_instance->functionMap[FUNCTIONCALL::DROP]();
		//s_instance->Drop();
		//
		//delete s_instance;
		//s_instance = nullptr;
		return true;
	}
	return false;
}

//bool CLuaInterface::Init()
//{
//
//
//	bool result = false;
//
//
//	return false;
//}
//
//void CLuaInterface::Run()
//{
//}
//
//void CLuaInterface::Drop()
//{
//}

int CLuaInterface::GetIntValue(const char* _value)
{
	/*Read variables.*/
	/*Go into .lua to find the "_value".*/
	lua_getglobal(s_instance->luaState, _value);

	/*Extract value, -1 because we extract 1 value from lua_getglobal.*/
	return(lua_tointeger(s_instance->luaState, -1));
}

char CLuaInterface::GetCharValue(const char * _value)
{
	return 0;
}

void CLuaInterface::SaveIntValue(const char * _varName, const int _value, const bool _overwrite)
{
	lua_getglobal(s_instance->luaState, "SaveToLuaFile");
	char outputString[80];
	sprintf(outputString, "%s = %d\n", _varName, _value);
	lua_pushstring(s_instance->luaState, outputString);
	lua_pushinteger(s_instance->luaState, _overwrite);
	lua_call(s_instance->luaState, 2, 0); 
	std::cout << "....................";
}

void CLuaInterface::SaveFloatValue(const char * _varName, const float _value, const bool _overwrite)
{
	lua_getglobal(s_instance->luaState, "SaveToLuaFile");
	char outputString[80];
	sprintf(outputString, "%s = %6.4f\n", _varName, _value);
	lua_pushstring(s_instance->luaState, outputString);
	lua_pushinteger(s_instance->luaState, _overwrite);
	lua_call(s_instance->luaState, 2, 0);
	//lua_call(theLuaState, passInValueCount, ReturnValueCount);
	//lua_tonumber is to get the top of the stack.
}

CLuaInterface::CLuaInterface()
	: luaState(nullptr)
	, isInit(false)
{
}
