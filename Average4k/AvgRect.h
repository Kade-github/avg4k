#pragma once
#include "Object.h"
class AvgRect : public Object
{
public:
	int w = 0;
	int h = 0;


	AvgRect(float _x, float _y, int _w, int _h) : Object(x, y) {
		x = _x;
		y = _y;
		w = _w;
		h = _h;
	};

	void draw() {

		SDL_FRect rect;

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;

		SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, alpha);

		SDL_RenderFillRectF(Game::renderer, &rect);
	}
};


