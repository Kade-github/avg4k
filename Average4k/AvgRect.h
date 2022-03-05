#pragma once
#include "Game.h"
#include "Object.h"
#include "GL.h"
class AvgRect : public Object
{
public:
	int w = 0;
	int h = 0;
	bool border = false;
	bool drawCall = true;
	int thickness = 4;

	Color c;


	AvgRect(float _x, float _y, int _w, int _h) : Object(x, y) {
		x = _x;
		y = _y;
		w = _w;
		h = _h;
	};

	void draw() {
		if (!drawCall)
			return;

		Rect rect;
		Rect srcRect;
		rect.x = x;
		rect.y = y;
		if (!staticView)
		{
			rect.x -= Game::mainView.x;
			rect.y -= Game::mainView.y;
		}
		rect.w = w;
		rect.h = h;
		rect.r = c.r;
		rect.g = c.g;
		rect.b = c.b;
		rect.a = alpha;

		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 1;
		srcRect.h = 1;

		
		Rendering::PushQuad(&rect, &srcRect, NULL, GL::genShader);
	}
};


