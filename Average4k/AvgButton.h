#pragma once
#include "Object.h"
#include "SDL2_gfxPrimitives.h"
#include "Text.h"
class AvgButton : public Object
{
public:
	int w = 0;
	int h = 0;
	std::string text;
	Color borderColor;
	Color fillColor;

	AvgButton(float _x, float _y, int _w, int _h, std::string _text) : Object(x, y) {
		x = _x;
		y = _y;
		w = _w;
		h = _h;
		text = _text;
	};

	void draw() {

		SDL_FRect rect;

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;

		roundedBoxRGBA(Game::renderer, (x + w), y, x, (y + h), 4, fillColor.r, fillColor.g, fillColor.b, 255);
		roundedRectangleRGBA(Game::renderer, (x + w), y, x, (y + h), 4, borderColor.r, borderColor.g, borderColor.b, 255, 1);
		
	}
};