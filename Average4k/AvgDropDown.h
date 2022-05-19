#pragma once
#include "includes.h"
#include "Object.h"
#include "Texture.h"
#include "SaveFile.h"
#include "Text.h"
#include "AvgContainer.h"

class AvgDropDown : public Object {
public:
	std::vector<std::string> def;

	Text* textPart;

	std::vector<Text*> otherTexts;

	setting toModify;

	Texture* box;

	AvgDropDown(int _x, int _y, std::vector<std::string> _def)
	{
		x = _x;
		y = _y;
		def = _def;

		toModify.name = "none";

		box = Noteskin::getMenuElement(Game::noteskin, "MainMenu/Settings/dropdown.png");
		w = box->width;
		h = box->height;
	}


	void mouseDown()
	{
		int _x, _y;
		Game::GetMousePos(&_x, &_y);

		if (parent == NULL)
			return;

		float scrll = ((AvgContainer*)parent)->scrollAddition;

		int relX = _x - parent->x;
		int relY = _y - parent->y + scrll;

		if ((relX > x && relY > y) && (relX < x + w && relY < y + h))
		{
			
		}
	}

	void draw()
	{
		Rect dstRect;
		Rect arrowRect;
		Rect srcRect;


		dstRect.r = 255;
		dstRect.g = 255;
		dstRect.b = 255;

		dstRect.a = alpha;

		dstRect.x = x;
		dstRect.y = y;
		dstRect.w = w;
		dstRect.h = h;

		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = 1;

		Rendering::PushQuad(&dstRect, &srcRect, box, GL::genShader);

	}
};