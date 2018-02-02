#ifndef LUA_INTERFACE
#define LUA_INTERFACE

/*Include Lua's headers.*/
#include <lua.hpp>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include "FunctionPointerWrapper.h"

class CLuaInterface
{
public:

	enum FUNCTIONCALL
	{
		INIT = 0,
		RUN,
		DROP,
		OBJECT,
		TOTAL
	};
	virtual ~CLuaInterface();

	static CLuaInterface* GetInstance();
	static bool DropInstance();

	bool Init(void);
	std::map<FUNCTIONCALL, TFunction*>functionMap;
	/*Pointer to Lua State*/
	lua_State* luaState;
	/*Get Int Value.*/
	int GetIntValue(const char* _value);
	/*Get Char Value*/
	char GetCharValue(const char* _value);

	/*Save values.*/
	/*Save an integer value using the Lua Interface Class.*/
	void SaveIntValue(const char* _varName, const int _value, const bool _overwrite = NULL);
	/*Save an float value using the Lua Interface Class.*/
	void SaveFloatValue(const char* _varName, const float _value, const bool _overwrite = NULL);

	int Calculate(std::vector<int>&_test);
	friend int luaAdd(lua_State* _state);
	friend int LuaGenerateObj(lua_State * _state);
	friend int LuaGetMesh(lua_State * _state);

	/*Boolean to check if Lua Interface is initialised.*/
	bool isInit;
private:
	CLuaInterface();
protected:
	static CLuaInterface* s_instance;
};

#endif LUA_INTERFACE