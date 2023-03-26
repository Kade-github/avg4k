#pragma once
#include "includes.h"
#include "Display.h"
namespace Average4k::Lua::Base
{
	struct rect {
	public:
		AvgEngine::Render::Rect* base = NULL;

		float x = 0, y = 0;
		float w = 0, h = 0;
		float r = 255, g = 255, b = 255;
		float a = 1;
		float scale = 1;
		float deg = 0;

		rect() = default;

		float getX() { return x; };
		float getY() { return y; };
		float getW() { return w; };
		float getH() { return h; };
		float getR() { return r; };
		float getG() { return g; };
		float getB() { return b; };
		float getA() { return a; };
		float getS() { return scale; };
		float getD() { return deg; };

		void setX(float _x) { x = _x; if (base) base->x = x; };
		void setY(float x) { y = x; if (base) base->y = x; };
		void setW(float x) { 
			w = x; 
			if (base) 
				base->w = x;
		};
		void setH(float x) { h = x; if (base) base->h = x; };
		void setR(float x) { r = x; if (base) base->r = x; };
		void setG(float x) { g = x; if (base) base->g = x; };
		void setB(float x) { b = x; if (base) base->b = x; };
		void setA(float x) { a = x; if (base) base->a = x; };
		void setS(float x) { scale = x; if (base) base->scale = x; };
		void setD(float x) { deg = x; if (base) base->angle = x; };

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