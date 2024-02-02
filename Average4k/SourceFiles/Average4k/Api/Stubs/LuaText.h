/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_TEXT_H
#define AVG4K_LUA_TEXT_H

#pragma once
#include "../../A4kGame.h"
#include "LuaObject.h"
#include <AvgEngine/Base/Text.h>

#define SOL_NO_EXCEPTIONS 1
#define SOL_USE_LUA_HPP
#include <sol/sol.hpp>

namespace Average4k::Api::Stubs
{
	class LuaText : public LuaObject {
		AvgEngine::Base::Text* _base;
		std::string _font;
	public:
		LuaText(int x, int y, const std::string& font, const std::string& text, int size)
		{
			_font = _font;
			_base = new AvgEngine::Base::Text(x, y, A4kGame::gameInstance->skin.GetPath("Fonts/"), font, text, size);
			_baseObject = _base;
		}

		void setX(int x)
		{
			_base->transform.x = x;
		}

		int getX()
		{
			return _base->transform.x;
		}

		void setY(int y)
		{
			_base->transform.y = y;
		}

		int getY()
		{
			return _base->transform.y;
		}

		void setText(const std::string& text)
		{
			_base->text = text;
		}

		std::string getText()
		{
			return _base->text;
		}

		void setFont(const std::string& font)
		{
			_base->SetFont(A4kGame::gameInstance->skin.GetPath("Fonts/"), font);
			_font = font;
		}

		std::string getFont()
		{
			return _font;
		}

		void setSize(int size)
		{
			_base->size = size;
		}

		int getSize()
		{
			return _base->size;
		}

		void setColor(int r, int g, int b, int a)
		{
			_base->transform.r = r;
			_base->transform.g = g;
			_base->transform.b = b;
			_base->transform.a = a;
		}

		void setAngle(float angle)
		{
			_base->transform.angle = angle;
		}

		float getAngle()
		{
			return _base->transform.angle;
		}

		int getWidth()
		{
			return _base->transform.w;
		}

		int getHeight()
		{
			return _base->transform.h;
		}

		static void Register(sol::state& state)
		{
			state.new_usertype<LuaText>("Text",
				sol::constructors<LuaText(int, int, const std::string&, const std::string&, int)>(),
				"x", sol::property(&LuaText::getX, &LuaText::setX),
				"y", sol::property(&LuaText::getY, &LuaText::setY),
				"width", sol::readonly_property(&LuaText::getWidth),
				"height", sol::readonly_property(&LuaText::getHeight),
				"text", sol::property(&LuaText::getText, &LuaText::setText),
				"font", sol::property(&LuaText::getFont, &LuaText::setFont),
				"size", sol::property(&LuaText::getSize, &LuaText::setSize),
				"color", &LuaText::setColor,
				"angle", sol::property(&LuaText::getAngle, &LuaText::setAngle),
				sol::base_classes, sol::bases<LuaObject>()
			);
		}
		

	};
}

#endif