#pragma once

#include <string>

class UIObj abstract
{
protected:


public:
	//Position
	float x;
	float y;

	float defaultX;
	float defaultY;

	float scale_x;
	float scale_y;

	float defaultScaleX;
	float defaultScaleY;

	float dir_x;
	float dir_y;

	UIObj() : UIObj("") {}
	UIObj(std::string name) : x(0), y(0), scale_x(1), scale_y(1), dir_x(1), dir_y(0), name(name) {}
	virtual ~UIObj() {}

	UIObj& set_x(float x) { this->defaultX = this->x = x; return *this; }
	UIObj& set_y(float y) { this->defaultY = this->y = y; return *this; }

	UIObj& set_scale_x(float x) { this->defaultScaleX = this->scale_x = x; return *this; }
	UIObj& set_scale_y(float y) { this->defaultScaleY = this->scale_y = y; return *this; }

	UIObj& set_dir_x(float x) { this->dir_x = x; return *this; }
	UIObj& set_dir_y(float y) { this->dir_y = y; return *this; }

	std::string name;
};