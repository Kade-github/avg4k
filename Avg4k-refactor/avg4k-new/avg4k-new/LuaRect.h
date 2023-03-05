#pragma once
#include "includes.h"
#include "Display.h"
namespace Average4k::Lua::Base
{
	struct rect {
	public:
		float x = 0, y = 0;
		float w = 0, h = 0;
		float r = 0, g = 0, b = 0;
		float a = 1;
		float scale = 1;
		float deg = 0;

		rect() = default;

		rect(float _x, float _y)
		{
			x = _x;
			y = _y;
		}

		rect(float _x, float _y, float _w, float _h)
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
		}

		rect(float _x, float _y, float _w, float _h, float _r, float _g, float _b, float _a)
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}

		rect(float _x, float _y, float _w, float _h, float _r, float _g, float _b, float _a, float _s, float _d)
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
			r = _r;
			g = _g;
			b = _b;
			a = _a;
			scale = _s;
			deg = _d;
		}

		AvgEngine::Render::Rect convert()
		{
			return AvgEngine::Render::Rect(x, y, w, h, r, g, b, a, scale, deg);
		}
	};
}