#pragma once

#include "UIObj.h"
#include "Mesh.h"
#include "MeshBuilder.h"
#include "Mesh.h"

class Button : public UIObj
{
public:
	Mesh* image;
	//std::string text;

	Button() : image(nullptr) {}
	Button(std::string name) : UIObj(name), image(nullptr) {}

	bool CheckCollision(float x, float y)
	{
		//std::cout << -scale_x + this->x << " , " << x << " , " << scale_x + this->x << std::endl;
		//std::cout << -scale_y + this->y << " , " << y << " , " << scale_y + this->y << std::endl;

		if (-scale_x * 0.5f + this->x < x && x < scale_x  * 0.5f + this->x &&
			-scale_y * 0.5f + this->y < y && y < scale_y * 0.5f + this->y)
		{
			return true;
		}
		return false;
	}
	
	Button& set_image(const char* file) {}
	//Button& set_text(const char* text) { this->text = text; }
};