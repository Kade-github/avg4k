/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_TEXT_H
#define AVG4K_LUA_TEXT_H

#pragma once
#include "../../A4kGame.h"
#include "LuaObject.h"
#include "../../Objects/UnicodeText.h"

#define SOL_NO_EXCEPTIONS 1
#define SOL_USE_LUA_HPP
#include <sol/sol.hpp>

namespace Average4k::Api::Stubs
{
	class LuaText : public LuaObject {
		Average4k::Objects::UnicodeText* _base;
		std::string _font;
	public:
		LuaText(float x, float y, const std::string& font, const std::wstring& text, int size)
		{
			_font = _font;
			_base = new Average4k::Objects::UnicodeText(x, y, A4kGame::gameInstance->skin.GetPath("Fonts/"), font, text, size);
			_baseObject = _base;
		}

		void setX(float x)
		{
			if (_base == NULL)
				return;
			_base->transform.x = x;
		}

		float getX()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.x;
		}

		void setY(float y)
		{
			if (_base == NULL)
				return;
			_base->transform.y = y;
		}

		float getY()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.y;
		}

		void setText(const std::wstring& text)
		{
			if (_base == NULL)
				return;
			_base->text = text;
		}

		std::wstring getText()
		{
			if (_base == NULL)
				return L"";
			return _base->text;
		}

		void setFont(const std::string& font)
		{
			if (_base == NULL)
				return;
			_base->SetFont(A4kGame::gameInstance->skin.GetPath("Fonts/"), font);
			_font = font;
		}

		std::string getFont()
		{
			if (_base == NULL)
				return "";
			return _font;
		}

		void setSize(int size)
		{
			if (_base == NULL)
				return;
			_base->size = size;
		}

		int getSize()
		{
			if (_base == NULL)
				return 0;
			return _base->size;
		}

		void setColor(int r, int g, int b, float a)
		{
			if (_base == NULL)
				return;
			_base->transform.r = r;
			_base->transform.g = g;
			_base->transform.b = b;
			_base->transform.a = a;
		}

		void setAngle(float angle)
		{
			if (_base == NULL)
				return;
			_base->transform.angle = angle;
		}

		float getAngle()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.angle;
		}

		int getWidth()
		{
			if (_base == NULL)
				return 0;

			return _base->transform.w;
		}

		int getHeight()
		{
			if (_base == NULL)
				return 0;

			return _base->transform.h;
		}

		std::string getTag()
		{
			if (_base == NULL)
				return "";
			return _base->tag;
		}

		void setTag(const std::string& tag)
		{
			if (_base == NULL)
				return;
			_base->tag = tag;
		}

		bool getCentered()
		{
			if (_base == NULL)
				return false;
			return _base->centerLines;
		}

		void setCentered(bool centered)
		{
			if (_base == NULL)
				return;
			_base->centerLines = centered;
		}

		static void Register(sol::state& state)
		{
			state.new_usertype<LuaText>("Text",
				sol::constructors<LuaText(int, int, const std::string&, const std::wstring&, int)>(),
				"tag", sol::property(&LuaText::getTag, &LuaText::setTag),
				"x", sol::property(&LuaText::getX, &LuaText::setX),
				"y", sol::property(&LuaText::getY, &LuaText::setY),
				"width", sol::readonly_property(&LuaText::getWidth),
				"height", sol::readonly_property(&LuaText::getHeight),
				"text", sol::property(&LuaText::getText, &LuaText::setText),
				"font", sol::property(&LuaText::getFont, &LuaText::setFont),
				"size", sol::property(&LuaText::getSize, &LuaText::setSize),
				"setColor", &LuaText::setColor,
				"angle", sol::property(&LuaText::getAngle, &LuaText::setAngle),
				"center", sol::property(&LuaText::getCentered, &LuaText::setCentered),
				sol::base_classes, sol::bases<LuaObject>()
			);
		}
		

	};
}

#endif