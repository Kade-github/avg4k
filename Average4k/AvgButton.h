#pragma once
#include "Object.h"
#include "Text.h"
#include <SDL2_gfxPrimitives.h>

typedef void(__cdecl* clickCallback)();

class AvgButton : public Object
{
public:
	int w = 0;
	int h = 0;
	int borderSize = 1;
	Color borderColor;
	Color defColor;
	Color fillColor;
	Color fontColor;
	Color hoverColor;
	Text* text;
	clickCallback callback;

	bool mouse = false;
	bool hovered = false;
	bool selected = false;

	AvgButton(float _x, float _y, int _w, int _h, std::string _text, int fontSize, std::string font, clickCallback callback) : Object(x, y) {

	};

	virtual ~AvgButton()
	{
		
	}

	virtual void clicked()
	{

	}

	virtual void draw() {


	}
};