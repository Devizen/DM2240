#pragma once
#include "UIObj.h"
#include "Mesh.h"
#include "MeshBuilder.h"
#include "Mesh.h"

class TextField : public UIObj
{
public:
	Mesh* background;
	std::string text;
	int asciiText;
	Color textColor;
	//std::string text;
	float textScale;

	//TextField() : background(nullptr), text("Text"), textColor(0, 0, 0), textScale(1.0f) { }
	TextField(const std::string& name, Mesh* background, const std::string& text, Color& color, float textScale) : UIObj(name), background(background), text(text), textColor(color), textScale(textScale), asciiText(0) {}

	//Button& set_text(const char* text) { this->text = text; }
};