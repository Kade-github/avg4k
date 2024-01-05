/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_RECT_H
#define AVG4K_LUA_RECT_H

#pragma once

#include "../AvgLuaFile.h"
#include <AvgEngine/Render/Display.h>

namespace Average4k::Api
{
	class LuaAvgRect : public AvgEngine::Render::Rect
	{
	public:

		LuaAvgRect(int x, int y, int w, int h) : AvgEngine::Render::Rect(x, y, w, h)
		{

		}

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

		int getW() {
			return w;
		}

		int getH() {
			return h;
		}

		int getR() {
			return r;
		}

		int getG() {
			return g;
		}

		int getB() {
			return b;
		}

		float getA() {
			return a;
		}

		float getAngle() {
			return angle;
		}

		void setX(int x) {
			this->x = x;
		}

		void setY(int y) {
			this->y = y;
		}

		void setW(int w) {
			this->w = w;
		}

		void setH(int h) {
			this->h = h;
		}

		void setR(int r) {
			this->r = r;
		}

		void setG(int g) {
			this->g = g;
		}

		void setB(int b) {
			this->b = b;
		}

		void setA(float a) {
			this->a = a;
		}

		void setAngle(float angle) {
			this->angle = angle;
		}

		static void registerLua(sol::state& state)
		{
			state.new_usertype<LuaAvgRect>("Rect",
				sol::constructors<LuaAvgRect(int, int, int, int)>(),
				"x", sol::property(&getX, &setX),
				"y", sol::property(&getY, &setY),
				"w", sol::property(&getW, &setW),
				"h", sol::property(&getH, &setH),
				"r", sol::property(&getR, &setR),
				"g", sol::property(&getG, &setG),
				"b", sol::property(&getB, &setB),
				"a", sol::property(&getA, &setA),
				"angle", sol::property(&getAngle, &setAngle)
			);
		}
		
	};

}

#endif