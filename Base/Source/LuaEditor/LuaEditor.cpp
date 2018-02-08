#include "LuaEditor.h"
#include <iostream>
#include <fstream>

LuaEditor* LuaEditor::s_instance = 0;

LuaEditor::~LuaEditor()
{
}

LuaEditor * LuaEditor::GetInstance()
{
	if (s_instance == nullptr)
		s_instance = new LuaEditor();
	return s_instance;
}

void LuaEditor::ReadLuaScript(const std::string _path)
{
	/*Ensure that stringList is completely cleared.*/
	while (!line.empty())
	{
		delete line.back();
		line.back() = nullptr;
		line.pop_back();
	}

	std::ifstream file(_path);
	std::string content = "";
	std::string data = "";
	std::string header = "\n<START READING .LUA FILE FROM: " + _path + '>';
	std::string footer = "<END READING .LUA FILE FROM: " + _path + ">\n";

	/*Get the entire .LUA data, line by line.*/
	while (getline(file, content))
	{
		line.push_back(new std::string(content));
	}

	std::cout << header << std::endl;
	/*Prompt notification for no file exist.*/
	if (!file.is_open())
	{
		std::cout << "<Lua Script does not exist at: " << _path << '>' << std::endl;
	}
	/*Prompt notification for no message loaded.*/
	else if (line.empty())
	{
		std::cout << "<Lua Script is empty.>" << std::endl;
		std::cout << "<No data loaded.>" << std::endl;
	}
	/*Prompt notification for message(s) loaded.*/
	else
	{
		size_t tempSize = line.size();
		for (size_t i = 0; i < tempSize; ++i)
		{
			std::cout << (*line[i]) << std::endl;
		}
		std::cout << "<Total lines loaded: " << std::to_string(line.size()) << '>' << std::endl;
	}
	std::cout << footer << std::endl;
	file.clear();
	file.close();
}

LuaEditor::LuaEditor() :
	isToggle(false)
	, currentLoadProgress(0)
	, completeLoadProgress(0)
{
	message = new std::string();
}
