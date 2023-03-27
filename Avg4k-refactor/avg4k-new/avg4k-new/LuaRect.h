#pragma once
#include "includes.h"
#include "Display.h"
namespace Average4k::Lua::Base
{
	struct rect {
	public:
		AvgEngine::Render::Rect* base = NULL;

		double x = 0, y = 0;
		double w = 0, h = 0;
		double r = 255, g = 255, b = 255;
		double a = 1;
		double scale = 1;
		double deg = 0;

		rect() = default;

		double getX() { return x; };
		double getY() { return y; };
		double getW() { return w; };
		double getH() { return h; };
		double getR() { return r; };
		double getG() { return g; };
		double getB() { return b; };
		double getA() { return a; };
		double getS() { return scale; };
		double getD() { return deg; };

		void setX(double _x) { 
			x = _x; 
			if (base) base->x = x; };
		void setY(double x) { y = x; if (base) base->y = x; };
		void setW(double x) { 
			w = x; 
			if (base) 
				base->w = x;
		};
		void setH(double x) { h = x; if (base) base->h = x; };
		void setR(double x) { r = x; if (base) base->r = x; };
		void setG(double x) { g = x; if (base) base->g = x; };
		void setB(double x) { b = x; if (base) base->b = x; };
		void setA(double x) { a = x; if (base) base->a = x; };
		void setS(double x) { scale = x; if (base) base->scale = x; };
		void setD(double x) { deg = x; if (base) base->angle = x; };

		rect(double _x, double _y)
		{
			x = _x;
			y = _y;
		}

		rect(double _x, double _y, double _w, double _h)
		{
			x = _x;
			y = _y;
			w = _w;
			h = _h;
		}

		rect(double _x, double _y, double _w, double _h, double _r, double _g, double _b, double _a)
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

		rect(double _x, double _y, double _w, double _h, double _r, double _g, double _b, double _a, double _s, double _d)
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