#pragma once
#include "Object.h"
#include "SDL2_gfxPrimitives.h"
#include "Text.h"
class AvgButton : public Object
{
public:
	int w = 0;
	int h = 0;
	int borderSize = 1;
	Color borderColor;
	Color fillColor;
	Color fontColor;
	Color hoverColor;
	Text* text;

	AvgButton(float _x, float _y, int _w, int _h, std::string _text, int fontSize, std::string font) : Object(x, y) {
		x = _x;
		y = _y;
		w = _w;
		h = _h;
		text = new Text(x,y,_text, fontSize,font);
		text->create();
		add(text);
	};


	void hover()
	{

	}


	void clicked()
	{

	}

	void draw() {

		SDL_FRect rect;

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;

		if (text->text != "")
		{
			if (text->color.r != fontColor.r || text->color.g != fontColor.g || text->color.b != fontColor.b)
			{
				text->color = fontColor;
				text->setText(text->text);
			}
			text->setX((x + (w / 2)) - (text->surfW / 2));
			text->setY(y + (text->surfH / 2));
			text->alpha = alpha;
		}

		int r = roundedBoxRGBA(Game::renderer, (x + w), y, x, (y + h), 4, fillColor.r, fillColor.g, fillColor.b, alpha);
		int rr = roundedRectangleRGBA(Game::renderer, (x + w), y, x, (y + h), 4, borderColor.r, borderColor.g, borderColor.b, alpha, borderSize);
	}
};