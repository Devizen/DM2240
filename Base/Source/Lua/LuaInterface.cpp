#include "LuaInterface.h"
#include <iostream>
#include <deque>
#include "MeshBuilder.h"
#include "Mesh.h"
#include "LoadTGA.cpp"
#include "../LuaEditor/LuaEditor.h"
CLuaInterface* CLuaInterface::s_instance = 0;

CLuaInterface::~CLuaInterface()
{
}

CLuaInterface * CLuaInterface::GetInstance()
{
	if (s_instance == nullptr)
	{
		s_instance = new CLuaInterface();
	}
	return s_instance;
}

bool CLuaInterface::DropInstance()
{
	if (s_instance)
	{
		/*Drop the Lua Interface Class.*/
		s_instance->functionMap[FUNCTIONCALL::DROP]->Run();
		return true;
	}
	return false;
}

bool CLuaInterface::Init(void)
{
	for (int i = 0; i < static_cast<int>(FUNCTIONCALL::TOTAL); ++i)
	{
		if (i == static_cast<int>(FUNCTIONCALL::INIT))
		{
			std::cout << "<Attemping to Initialise Lua State.>" << std::endl;
			isInit = false;
			/*Create Init function.*/
			functionMap.insert(std::make_pair(FUNCTIONCALL::INIT, new FunctionPointerWrapper<void>([this]()->void {
				/*Create Lua state.*/
				luaState = lua_open();

				if (luaState)
				{
					/*Load Lua auxiliary libraries.*/
					luaL_openlibs(luaState);

					luaL_dofile(luaState, "Image//DM2240.lua");
					s_instance->isInit = true;
					std::cout << "<Loaded Lua Script from " << "Image//DM2240.lua>" << std::endl;
				}
				if (!s_instance->isInit)
					std::cout << "<Failed to initialise Lua State.>" << std::endl;
			})));
		}
		else if (i == static_cast<int>(FUNCTIONCALL::RUN))
		{
			isInit = false;
			/*Create Run function.*/
			functionMap.insert(std::make_pair(RUN, new FunctionPointerWrapper<void>([this]()->void {
				/*Return if the luaState is not initialised.*/
				if (luaState == nullptr)
					return;

				/*Read variables.*/
				/*Go into .lua to find the "title".*/
				lua_getglobal(luaState, "title");

				/*Extract value, -1 because we extract 1 value from lua_getglobal.*/
				const char* title = lua_tostring(luaState, -1);

				lua_getglobal(luaState, "width");
				int width = lua_tointeger(luaState, -1);

				lua_getglobal(luaState, "height");
				int height = lua_tointeger(luaState, -1);

				/*Display the values on screen.*/
				std::cout << title << std::endl;
				std::cout << "--------------------------------------" << std::endl;
				std::cout << "Width of Screen: " << width << std::endl;
				std::cout << "Height of Screen: " << height << std::endl;

			})));
		}
		else if (i == static_cast<int>(FUNCTIONCALL::DROP))
		{
			isInit = false;
			/*Create Run function.*/
			functionMap.insert(std::make_pair(FUNCTIONCALL::DROP, new FunctionPointerWrapper<void>([this]()->void {
				if (luaState)
					lua_close(luaState);

			})));
		}
		else if (i == static_cast<int>(FUNCTIONCALL::OBJECT))
		{
			isInit = false;
			/*Create object(s) initialisation.*/
			functionMap.insert(std::make_pair(FUNCTIONCALL::OBJECT, new FunctionPointerWrapper<void(std::vector<std::string>&_obj, std::vector<std::string>&_tex)>(
				[this](std::vector<std::string>&_obj, std::vector<std::string>&_tex)->void
			{
				if (luaState)
				{
					/*Load Lua auxiliary libraries.*/
					luaL_openlibs(luaState);
					LuaEditor* luaEditor = LuaEditor::GetInstance();
					luaEditor->ReadLuaScript("Lua//LuaGenerateObj.lua");
					size_t generateObjCount = luaEditor->GetLine().size();
					luaEditor->ReadLuaScript("Lua//LuaGetMesh.lua");
					size_t getMeshCount = luaEditor->GetLine().size();

					for (size_t i = 0; i < generateObjCount; ++i)
					{
						std::string generateObj = "GenerateOBJ";
						generateObj += std::to_string(i);
						lua_register(luaState, generateObj.c_str(), LuaGenerateObj);
					}

					for (size_t i = 0; i < generateObjCount; ++i)
					{
						std::string getMesh = "GetMesh";
						getMesh += std::to_string(i);
						lua_register(luaState, getMesh.c_str(), LuaGetMesh);
					}

					luaL_dofile(luaState, "Lua//LuaGenerateObj.lua");
					luaL_dofile(luaState, "Lua//LuaGetMesh.lua");
					s_instance->isInit = true;
					std::cout << "<Load Lua Script from " << "Lua//LuaGenerateObj.lua" << std::endl;
					std::cout << "<Load Lua Script from " << "Lua//LuaGetMesh.lua" << std::endl;
				}
				if (!s_instance->isInit)
					std::cout << "<Failed to initialise Lua State.>" << std::endl;
			}
			)));
		}
	}
	return false;
}

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

int CLuaInterface::Calculate(std::vector<int>& _test)
{
	size_t tempSize = _test.size();
	int tempValue = 0;
	for (size_t i = 0; i < tempSize; ++i)
	{
		tempValue += _test[i];
	}
	return tempValue;
}

int luaAdd(lua_State * _state)
{
	int tempSize = lua_gettop(_state);
	std::vector<int>testValue;
	for (int i = 1; i < tempSize; ++i)
	{
		testValue.push_back(lua_tointeger(_state, i));
	}
	lua_pushinteger(_state, CLuaInterface::GetInstance()->Calculate(testValue));
	return 1;
}

int LuaGenerateObj(lua_State * _state)
{
	int tempSize = lua_gettop(_state);
	std::deque<std::string>objField;
	//int textureID = 0;
	for (int i = 1; i <= tempSize; ++i)
	{
		objField.push_back(lua_tostring(_state, i));
	}
	if (MeshBuilder::GetInstance()->GenerateOBJ(objField[0].c_str(), objField[1].c_str()))
		std::cout << "<LuaGenerateObj loaded " << objField[0].c_str() << " " << objField[1].c_str() << ">" << std::endl;
	else
		std::cout << "<LuaGenerateObj failed to load " << objField[0].c_str() << " " << objField[1].c_str() << ">" << std::endl;
	return 0;
}

int LuaGetMesh(lua_State * _state)
{
	int tempSize = lua_gettop(_state);
	std::deque<std::string>objField;
	int textureID = 0;
	for (int i = 1; i <= tempSize; ++i)
	{
		if (i == 2)
		{
			textureID = lua_tointeger(_state, i);
			continue;
		}
		objField.push_back(lua_tostring(_state, i));
	}
	if (MeshBuilder::GetInstance()->GetMesh(objField[0].c_str())->textureID[textureID] = LoadTGA(objField[1].c_str()))
		std::cout << "<LuaGetMesh loaded " << objField[0].c_str() << " " << std::to_string(textureID) << " " << objField[1].c_str() << ">" << std::endl;
	else
		std::cout << "<LuaGetMesh failed to load " << objField[0].c_str() << " " << std::to_string(textureID) << " " << objField[1].c_str() << ">" << std::endl;
	return 0;
}

CLuaInterface::CLuaInterface()
	: luaState(nullptr)
	, isInit(false)
{
}
