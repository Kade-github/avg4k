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
		double scale = 1.0;
		double deg = 0;

		rect() = default;

		double getX() { 
			if (base)
				return base->x;
			return x;
		};
		double getY() {
			if (base)
				return base->y;
			return y;
		};
		double getW() {
			if (base)
				return base->w;
			return w;
		};
		double getH() {
			if (base)
				return base->h;
			return h;
		};
		double getR() {
			if (base)
				return base->r;
			return r;
		};
		double getG() {
			if (base)
				return base->g;
			return g;
		};
		double getB() {
			if (base)
				return base->b;
			return b;
		};
		double getA() {
			if (base)
				return base->a;
			return a;
		};
		double getS() {
			if (base)
				return base->scale;
			return scale;
		};
		double getD() {
			if (base)
				return base->angle;
			return deg;
		};

		void setX(double _x) { 
			x = _x; 
			if (base) 
				base->x = _x; 
		};
		void setY(double _x) { y = _x; if (base) base->y = _x; };
		void setW(double _x) { 
			w = _x; 
			if (base) 
				base->w = _x;
		};
		void setH(double _x) { h = _x; if (base) base->h = _x; };
		void setR(double _x) { r = _x; if (base) base->r = _x; };
		void setG(double _x) { g = _x; if (base) base->g = _x; };
		void setB(double _x) { b = _x; if (base) base->b = _x; };
		void setA(double _x) { a = _x; if (base) base->a = _x; };
		void setS(double _x) { scale = _x; if (base) base->scale = _x; };
		void setD(double _x) { deg = _x; if (base) base->angle = _x; };

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