#ifndef LUAEDITOR_H
#define LUAEDITOR_H

#include <string>
#include <deque>
#include <functional>

/*
Done by Lai Wei Jing, Eugene and Ow Hong Yu.
2nd February 2018.
*/

class LuaEditor
{
public:
	~LuaEditor();
	/*Get Singleton instance.*/
	static LuaEditor* GetInstance();
	/*Open and save the content of .LUA file to line container.*/
	void ReadLuaScript(const std::string _path);
	void WriteLuaScript(std::string _path);

	/*Get container for editing.*/
	std::deque<std::string*>& GetLine(void) { return line; }

	/*Get Status of LuaEditor.*/
	const bool& GetIsToggle(void) { return isToggle; }

	/*Set Status of LuaEditor.*/
	void SetIsToggle(bool _isToggle) { isToggle = _isToggle; }

	/*Get input field.*/
	std::deque<std::string>list;
	//std::function<std::string(unsigned int)>& GetInputField());
private:
	/*Instance of LuaEditor in private so that no one can access directly.*/
	static LuaEditor* s_instance;

	/*Constructor of LuaEditor in private so that no one can create LuaEditor instance without using GetInstance().*/
	LuaEditor();

	/*To check if Lua Editor was toggled.*/
	bool isToggle;

	std::deque<std::string*>line;

	std::function<std::string(unsigned int)>inputSelection;
protected:
};

#endif