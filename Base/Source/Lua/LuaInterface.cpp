#include "LuaInterface.h"
#include <iostream>
#include <deque>
#include "MeshBuilder.h"
#include "Mesh.h"
#include "LoadTGA.cpp"
#include "../LuaEditor/LuaEditor.h"
#include "../Application.h"
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
					lua_register(luaState, "luaAdd", luaAdd);

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
			//functionMap.insert(std::make_pair(FUNCTIONCALL::OBJECT, new FunctionPointerWrapper<void(std::vector<std::string>&_obj, std::vector<std::string>&_tex)>(
			//	[this](std::vector<std::string>&_obj, std::vector<std::string>&_tex)->void
			functionMap.insert(std::make_pair(FUNCTIONCALL::OBJECT, new FunctionPointerWrapper<void>([this]()->void 
			{
				if (luaState)
				{
					/*Load Lua auxiliary libraries.*/
					luaL_openlibs(luaState);

					/*Read how many lines in lua script so that the luaState will know how many lines to retrieve.*/
					LuaEditor* luaEditor = LuaEditor::GetInstance();

					/*String for all lua script path.
					One time initialisation for optimisation because it will be used a few times below.*/
					std::map<std::string, std::function<lua_CFunction(void)>>functionToRegister;

					/*Path and Function Call Name.*/
					std::deque<std::pair<std::string, std::string>>pathAndName;

					/*Generate Axes.*/
					//lua_register(luaState, "GenerateAxes", LuaGenerateAxes);
					pathAndName.push_back(std::make_pair("Lua//LuaGenerateAxes.lua", "GenerateAxes"));
					//functionToRegister[pathAndName.back().first] = [](void)->lua_CFunction { return LuaGenerateAxes; };

					/*Generate Object(s).*/
					//lua_register(luaState, "GenerateOBJ", LuaGenerateObj);

					pathAndName.push_back(std::make_pair("Lua//LuaGenerateObj.lua", "GenerateOBJ"));
					//functionToRegister[pathAndName.back().first] = [](void)->lua_CFunction { return LuaGenerateObj; };

					/*Generate Quad(s).*/
					//lua_register(luaState, "GenerateQuad", LuaGenerateQuad);
					pathAndName.push_back(std::make_pair("Lua//LuaGenerateQuad.lua", "GenerateQuad"));
					//functionToRegister[pathAndName.back().first] = [](void)->lua_CFunction { return LuaGenerateQuad; };

					/*Generate Skyplane(s).*/
					//lua_register(luaState, "GenerateSkyplane", LuaGenerateSkyplane);
					pathAndName.push_back(std::make_pair("Lua//LuaGenerateSkyplane.lua", "GenerateSkyplane"));
					//functionToRegister[pathAndName.back().first] = [](void)->lua_CFunction { return LuaGenerateSkyplane; };

					/*Generate Text(s).
					Not needed anymore because this was initialised at the start of the program.*/
					/*pathAndName.push_back(std::make_pair("Lua//LuaGenerateText.lua", "GenerateText"));
					functionToRegister[pathAndName.back().first] = [](void)->lua_CFunction { return LuaGenerateText; };*/

					/*Getting Mesh for Object(s).
					For safety reason, this should be placed last because it will get mesh which will crash the application if the mesh has not initialise.*/
					
					//lua_register(luaState, "GetMesh", LuaGetMesh);
					
					pathAndName.push_back(std::make_pair("Lua//LuaGetMesh.lua", "GetMesh"));
					//functionToRegister[pathAndName.back().first] = [](void)->lua_CFunction { return LuaGetMesh; };

					size_t totalScriptToLoad = pathAndName.size();

					luaEditor->SetCurrentLoadProgress(0);
					luaEditor->SetCompleteLoadProgress(0);
					for (size_t i = 0; i < totalScriptToLoad; ++i)
					{
						/*Load the intended lua script.*/
						luaEditor->ReadLuaScript(pathAndName[i].first);
						/*Get the amount of line(s) to loop through.*/
						size_t tempCount = luaEditor->GetLine().size();
						luaEditor->AddCompleteLoadProgress(static_cast<float>(tempCount));
						/*Add for reading and doing file.*/
						luaEditor->AddCompleteLoadProgress(static_cast<float>(2.f));
					}

					//for (size_t i = 0; i < totalScriptToLoad; ++i)
					//{
					//	/*Load the intended lua script.*/
					//	luaEditor->ReadLuaScript(pathAndName[i].first);
					//	luaEditor->AddCurrentLoadProgress(1);

					//	/*Get the amount of line(s) to loop through.*/
					//	size_t tempCount = luaEditor->GetLine().size();
					//
					//	for (size_t j = 0; j < tempCount; ++j)
					//	{
					//		std::string tempStr = pathAndName[i].second;
					//		//tempStr += std::to_string(j);
					//		/*Register the name to look for using the function binded.*/
					//		std::string tempString = "<Registering " + tempStr + " to function " + pathAndName[i].first;
					//		lua_register(luaState, tempStr.c_str(), functionToRegister[pathAndName[i].first]());

					//		LuaEditor::GetInstance()->SetMessage(tempString);
					//		Application::GetInstance().Iterate();
					//		luaEditor->AddCurrentLoadProgress(1);
					//		break;
					//	}
					//	/*Execute loading script.*/
					//	luaL_dofile(luaState, pathAndName[i].first.c_str());
					//	luaEditor->AddCurrentLoadProgress(1);

					//	std::string tempString = "<Load Lua Script from " + pathAndName[i].first;
					//	std::cout << "<Load Lua Script from " << pathAndName[i].first << std::endl;

					//	LuaEditor::GetInstance()->SetMessage(tempString);
					//	Application::GetInstance().Iterate();
					//}

					lua_register(luaState, "GenerateAxes", LuaGenerateAxes);
					lua_register(luaState, "GenerateOBJ", LuaGenerateObj);
					lua_register(luaState, "GenerateQuad", LuaGenerateQuad);
					lua_register(luaState, "GenerateSkyplane", LuaGenerateSkyplane);
					lua_register(luaState, "GetMesh", LuaGetMesh);

					luaL_dofile(luaState, "Lua//LuaGenerateAxes.lua");
					luaL_dofile(luaState, "Lua//LuaGenerateOBJ.lua");
					luaL_dofile(luaState, "Lua//LuaGenerateQuad.lua");
					luaL_dofile(luaState, "Lua//LuaGenerateSkyplane.lua");
					luaL_dofile(luaState, "Lua//LuaGetMesh.lua");
					s_instance->isInit = true;
				}
				if (!s_instance->isInit)
				{
					std::string tempString = "<Failed to initialise Lua State.>";

					std::cout << "<Failed to initialise Lua State.>" << std::endl;

					LuaEditor::GetInstance()->SetMessage(tempString);
					Application::GetInstance().Iterate();
				}
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
	/*Read variables.*/
	/*Go into .lua to find the "_value".*/
	lua_getglobal(s_instance->luaState, _value);

	/*Extract value, -1 because we extract 1 value from lua_getglobal.
	Takes index 0 because it is a char.*/
	return (lua_tostring(s_instance->luaState, -1))[0];
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
	{
		std::string tempString = "<LuaGenerateObj loaded " + objField[0] + " " + objField[1] + ">";
		std::cout << "<LuaGenerateObj loaded " << objField[0].c_str() << " " << objField[1].c_str() << ">" << std::endl;
		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
		
	}
	else
	{
		std::string tempString = "<LuaGenerateObj failed to load " + objField[0] + " " + objField[1] + ">";
		std::cout << "<LuaGenerateObj failed to load " << objField[0].c_str() << " " << objField[1].c_str() << ">" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
	return 0;
}

int LuaGenerateQuad(lua_State * _state)
{
	int tempSize = lua_gettop(_state);
	std::deque<std::string>quadField;
	std::deque<float>quadValue;

	for (int i = 1; i <= tempSize; ++i)
	{
		if (i == 1)
		{
			quadField.push_back(lua_tostring(_state, i));
			continue;
		}
		quadValue.push_back(static_cast<float>(lua_tointeger(_state, i)));
	}
	/*Error prevention.*/
	if (quadField.size() == 0)
	{
		quadField.clear();
		quadField.push_back("QUAD");
		std::cout << "<LuaGenerateQuad have no name, using default \"QUAD\" as name.>" << std::endl;
	}
	while (quadValue.size() < 4)
	{
		quadValue.push_back(1);
		std::cout << "<LuaGenerateQuad have lack of value, using default \"1\" as value.>" << std::endl;
	}

	if (MeshBuilder::GetInstance()->GenerateQuad(quadField[0].c_str(), Color(quadValue[0], quadValue[1], quadValue[2]), quadValue[3]))
	{
		std::string tempString = "<LuaGenerateQuad loaded " + quadField[0] + " Color(" + std::to_string(quadValue[0]) +
			"," + std::to_string(quadValue[1]) + "," + std::to_string(quadValue[2]) + ") Length " + std::to_string(quadValue[3]) + ">";

		std::cout << "<LuaGenerateQuad loaded " << quadField[0].c_str() << " Color(" << std::to_string(quadValue[0]) <<
			"," << std::to_string(quadValue[1]) << "," << std::to_string(quadValue[2]) << ") Length " << std::to_string(quadValue[3]) << ">" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
	else
	{
		std::string tempString = "<LuaGenerateQuad failed to load " + quadField[0] + " Color(" + std::to_string(quadValue[0]) +
			"," + std::to_string(quadValue[1]) + "," + std::to_string(quadValue[2]) + ") Length " + std::to_string(quadValue[3]) + ">";

		std::cout << "<LuaGenerateQuad failed to load " << quadField[0].c_str() << " Color(" << std::to_string(quadValue[0]) <<
			"," << std::to_string(quadValue[1]) << "," << std::to_string(quadValue[2]) << ") Length " << std::to_string(quadValue[3]) << ">" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
	return 0;
}

int LuaGenerateSphere(lua_State * _state)
{
	return 0;
}

int LuaGenerateCube(lua_State * _state)
{
	return 0;
}

int LuaGenerateCone(lua_State * _state)
{
	return 0;
}

int LuaGenerateRing(lua_State * _state)
{
	return 0;
}

int LuaGenerateTerrain(lua_State * _state)
{
	return 0;
}

int LuaGenerateRay(lua_State * _state)
{
	return 0;
}

int LuaGenerateSkyplane(lua_State * _state)
{
	int tempSize = lua_gettop(_state);
	std::deque<std::string>skyplaneField;
	std::deque<float>skyplaneValue;

	for (int i = 1; i <= tempSize; ++i)
	{
		if (i == 1)
		{
			skyplaneField.push_back(lua_tostring(_state, i));
			continue;
		}
		skyplaneValue.push_back(static_cast<float>(lua_tointeger(_state, i)));
	}
	/*Error prevention.*/
	if (skyplaneField.size() == 0)
	{
		skyplaneField.clear();
		skyplaneField.push_back("SKYPLANE");
		std::cout << "<LuaGenerateSkyplane have no name, using default \"SKYPLANE\" as name.>" << std::endl;
	}
	while (skyplaneValue.size() < 8)
	{
		if (skyplaneValue.size() == 3)
		{
			skyplaneValue.push_back(128);
			continue;
		}
		else if (skyplaneValue.size() == 4)
		{
			skyplaneValue.push_back(400);
			continue;
		}
		else if (skyplaneValue.size() == 5)
		{
			skyplaneValue.push_back(1000);
			continue;
		}
		skyplaneValue.push_back(1);
		std::cout << "<LuaGenerateSkyplane have lack of value, using default \"1\" as value.>" << std::endl;
	}

	if (MeshBuilder::GetInstance()->GenerateSkyPlane(skyplaneField[0].c_str(), Color(skyplaneValue[0], skyplaneValue[1], skyplaneValue[2]), skyplaneValue[3],
		skyplaneValue[4], skyplaneValue[5], skyplaneValue[6], skyplaneValue[7]))
	{
		std::string tempString = "<LuaGenerateSkyplane loaded " + skyplaneField[0] + " Color(" + std::to_string(skyplaneValue[0]) +
			"," + std::to_string(skyplaneValue[1]) + "," + std::to_string(skyplaneValue[2]) + ") Slices " + std::to_string(skyplaneValue[3]) +
			" Planet Radius " + std::to_string(skyplaneValue[4]) + " Atmosphere Radius " + std::to_string(skyplaneValue[5]) +
			" hTile " + std::to_string(skyplaneValue[6]) + " vTile " + std::to_string(skyplaneValue[7]) + ">";

		std::cout << "<LuaGenerateSkyplane loaded " << skyplaneField[0].c_str() << " Color(" << std::to_string(skyplaneValue[0]) <<
			"," << std::to_string(skyplaneValue[1]) << "," << std::to_string(skyplaneValue[2]) << ") Slices " << std::to_string(skyplaneValue[3]) <<
			" Planet Radius " << std::to_string(skyplaneValue[4]) << " Atmosphere Radius " << std::to_string(skyplaneValue[5]) <<
			" hTile " << std::to_string(skyplaneValue[6]) << " vTile " << std::to_string(skyplaneValue[7]) << ">" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
	else
	{
		std::string tempString = "<LuaGenerateSkyplane failed to load " + skyplaneField[0] + " Color(" + std::to_string(skyplaneValue[0]) +
			"," + std::to_string(skyplaneValue[1]) + "," + std::to_string(skyplaneValue[2]) + ") Slices " + std::to_string(skyplaneValue[3]) +
			" Planet Radius " + std::to_string(skyplaneValue[4]) + " Atmosphere Radius " + std::to_string(skyplaneValue[5]) +
			" hTile " + std::to_string(skyplaneValue[6]) + " vTile " + std::to_string(skyplaneValue[7]) + ">";

		std::cout << "<LuaGenerateSkyplane failed to load " << skyplaneField[0].c_str() << " Color(" << std::to_string(skyplaneValue[0]) <<
			"," << std::to_string(skyplaneValue[1]) << "," << std::to_string(skyplaneValue[2]) << ") Slices " << std::to_string(skyplaneValue[3]) <<
			" Planet Radius " << std::to_string(skyplaneValue[4]) << " Atmosphere Radius " << std::to_string(skyplaneValue[5]) <<
			" hTile " << std::to_string(skyplaneValue[6]) << " vTile " << std::to_string(skyplaneValue[7]) << ">" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
	return 0;
}

static int LuaGenerateAxes(lua_State * _state)
{
	int tempSize = lua_gettop(_state);
	std::deque<std::string>axesField;
	std::deque<float>axesValue;
	//int textureID = 0;
	for (int i = 1; i <= tempSize; ++i)
	{
		if (i == 1)
		{
			axesField.push_back(lua_tostring(_state, i));
			continue;
		}
		axesValue.push_back(static_cast<float>(lua_tointeger(_state, i)));
	}
	/*Error prevention.*/
	if (axesField.size() == 0)
	{
		axesField.clear();
		axesField.push_back("REFERENCE");
		std::cout << "<LuaGenerateAxes have no name, using default \"REFERENCE\" as name.>" << std::endl;
	}
	while (axesValue.size() < 3)
	{
		axesValue.push_back(0);
		std::cout << "<LuaGenerateAxes have lack of value, using default \"0\" as value.>" << std::endl;
	}

	if (MeshBuilder::GetInstance()->GenerateAxes(axesField[0].c_str(), axesValue[0], axesValue[1], axesValue[2]))
	{
		std::string tempString = "<LuaGenerateAxes loaded " + axesField[0] + " " + std::to_string(axesValue[0]) +
			" " + std::to_string(axesValue[1]) + " " + std::to_string(axesValue[2]) + ">";

		std::cout << "<LuaGenerateAxes loaded " << axesField[0].c_str() << " " << std::to_string(axesValue[0]) <<
			" " << std::to_string(axesValue[1]) << " " << std::to_string(axesValue[2]) << ">" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
	else
	{
		std::string tempString = "<LuaGenerateAxes failed to load " + axesField[0] + " " + std::to_string(axesValue[0]) +
			" " + std::to_string(axesValue[1]) + " " + std::to_string(axesValue[2]) + ">";

		std::cout << "<LuaGenerateAxes failed to load " << axesField[0].c_str() << " " << std::to_string(axesValue[0]) <<
			" " << std::to_string(axesValue[1]) << " " << std::to_string(axesValue[2]) << ">" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
	return 0;
}

int LuaGenerateCrossHair(lua_State * _state)
{
	return 0;
}

int LuaGenerateText(lua_State * _state)
{
	int tempSize = lua_gettop(_state);
	std::deque<std::string>textField;
	std::deque<float>textValue;

	for (int i = 1; i <= tempSize; ++i)
	{
		if (i == 1)
		{
			textField.push_back(lua_tostring(_state, i));
			continue;
		}
		textValue.push_back(static_cast<float>(lua_tointeger(_state, i)));
	}
	/*Error prevention.*/
	if (textField.size() == 0)
	{
		textField.clear();
		textField.push_back("TEXT");
		std::cout << "<LuaGenerateText have no name, using default \"TEXT\" as name.>" << std::endl;
	}
	while (textValue.size() < 2)
	{
		textValue.push_back(16);
		std::cout << "<LuaGenerateText have lack of value, using default \"16\" as value.>" << std::endl;
	}

	if (MeshBuilder::GetInstance()->GenerateText(textField[0].c_str(), textValue[0], textValue[1]))
	{
		std::string tempString = "<LuaGenerateText loaded " + textField[0] + " Size(" + std::to_string(textValue[0]) +
			"," + std::to_string(textValue[1]) + ")>";

		std::cout << "<LuaGenerateText loaded " << textField[0].c_str() << " Size(" << std::to_string(textValue[0]) <<
			"," << std::to_string(textValue[1]) << ")>" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
	else
	{
		std::string tempString = "<LuaGenerateText failed to load " + textField[0] + " Size(" + std::to_string(textValue[0]) +
			"," + std::to_string(textValue[1]) + ")>";

		std::cout << "<LuaGenerateText failed to load " << textField[0].c_str() << " Size(" << std::to_string(textValue[0]) <<
			"," << std::to_string(textValue[1]) << ")>" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
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
	if (MeshBuilder::GetInstance()->GetMesh(objField[0].c_str()) != nullptr)
	{
		MeshBuilder::GetInstance()->GetMesh(objField[0])->textureID[textureID] = LoadTGA(objField[1].c_str());
		std::string tempString = "<LuaGetMesh loaded " + objField[0] + " " + std::to_string(textureID) + " " + objField[1] + ">";
		std::cout << "<LuaGetMesh loaded " << objField[0].c_str() << " " << std::to_string(textureID) << " " << objField[1].c_str() << ">" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
	else
	{
		std::string tempString = "<LuaGetMesh failed to load " + objField[0] + " " + std::to_string(textureID) + " " + objField[1] + ">";
		std::cout << "<LuaGetMesh failed to load " << objField[0].c_str() << " " << std::to_string(textureID) << " " << objField[1].c_str() << ">" << std::endl;

		LuaEditor::GetInstance()->SetMessage(tempString);
		Application::GetInstance().Iterate();
	}
	return 0;
}

CLuaInterface::CLuaInterface()
	: luaState(nullptr)
	, isInit(false)
{
}
