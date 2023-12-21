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
		float scale = 1.0;
		float deg = 0;

		rect() = default;

		float getX() { 
			if (base)
				return base->x;
			return x;
		};
		float getY() {
			if (base)
				return base->y;
			return y;
		};
		float getW() {
			if (base)
				return base->w;
			return w;
		};
		float getH() {
			if (base)
				return base->h;
			return h;
		};
		float getR() {
			if (base)
				return base->r;
			return r;
		};
		float getG() {
			if (base)
				return base->g;
			return g;
		};
		float getB() {
			if (base)
				return base->b;
			return b;
		};
		float getA() {
			if (base)
				return base->a;
			return a;
		};
		float getS() {
			if (base)
				return base->scale;
			return scale;
		};
		float getD() {
			if (base)
				return base->angle;
			return deg;
		};

		void setX(float _x) { 
			x = _x; 
			if (base) 
				base->x = _x; 
		};
		void setY(float _x) { y = _x; if (base) base->y = _x; };
		void setW(float _x) { 
			w = _x; 
			if (base) 
				base->w = _x;
		};
		void setH(float _x) { h = _x; if (base) base->h = _x; };
		void setR(float _x) { r = _x; if (base) base->r = _x; };
		void setG(float _x) { g = _x; if (base) base->g = _x; };
		void setB(float _x) { b = _x; if (base) base->b = _x; };
		void setA(float _x) { a = _x; if (base) base->a = _x; };
		void setS(float _x) { 
			scale = _x; 
			if (base) 
				base->scale = _x; 
		};
		void setD(float _x) { deg = _x; if (base) base->angle = _x; };

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