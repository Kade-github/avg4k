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
		std::shared_ptr<Average4k::Objects::UnicodeText> _base;
		std::string _font;
	public:
		/// <summary>
		/// Create a new text object
		/// </summary>
		/// <param name="x">The x position</param>
		/// <param name="y">The y position</param>
		/// <param name="font">The font</param>
		/// <param name="text">The text</param>
		/// <param name="size">The size</param>
		LuaText(float x, float y, const std::string& font, const std::wstring& text, int size)
		{
			_font = _font;
			_base = std::make_shared<Average4k::Objects::UnicodeText>(x, y, A4kGame::gameInstance->skin.GetPath("Fonts/"), font, text, size);
			_baseObject = _base;
		}

		/// <summary>
		/// Set the x position of the text
		/// </summary>
		/// <param name="x">The x position</param>
		void setX(float x)
		{
			if (_base == NULL)
				return;
			_base->transform.x = x;
		}

		/// <summary>
		/// Get the x position of the text
		/// </summary>
		/// <returns>The x position</returns>
		float getX()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.x;
		}

		/// <summary>
		/// Set the y position of the text
		/// </summary>
		/// <param name="y">The y position</param>
		void setY(float y)
		{
			if (_base == NULL)
				return;
			_base->transform.y = y;
		}

		/// <summary>
		/// Get the y position of the text
		/// </summary>
		/// <returns>The y position</returns>
		float getY()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.y;
		}

		/// <summary>
		/// Set the text of the text
		/// </summary>
		/// <param name="text">The text</param>
		void setText(const std::wstring& text)
		{
			if (_base == NULL)
				return;
			_base->text = text;
		}

		/// <summary>
		/// Get the text of the text
		/// </summary>
		/// <returns>The text</returns>
		std::wstring getText()
		{
			if (_base == NULL)
				return L"";
			return _base->text;
		}

		/// <summary>
		/// Set the font of the text
		/// </summary>
		/// <param name="font">The font</param>
		void setFont(const std::string& font)
		{
			if (_base == NULL)
				return;
			_base->SetFont(A4kGame::gameInstance->skin.GetPath("Fonts/"), font);
			_font = font;
		}

		/// <summary>
		/// Get the font of the text
		/// </summary>
		/// <returns>The font</returns>
		std::string getFont()
		{
			if (_base == NULL)
				return "";
			return _font;
		}

		/// <summary>
		/// Set the size of the text
		/// </summary>
		/// <param name="size">The size</param>
		void setSize(int size)
		{
			if (_base == NULL)
				return;
			_base->size = size;
		}

		/// <summary>
		/// Get the size of the text
		/// </summary>
		/// <returns>The size</returns>
		int getSize()
		{
			if (_base == NULL)
				return 0;
			return _base->size;
		}

		/// <summary>
		/// Set the color of the text
		/// </summary>
		/// <param name="r">The red value</param>
		/// <param name="g">The green value</param>
		/// <param name="b">The blue value</param>
		void setColor(int r, int g, int b, float a)
		{
			if (_base == NULL)
				return;
			_base->transform.r = r;
			_base->transform.g = g;
			_base->transform.b = b;
			_base->transform.a = a;
		}

		/// <summary>
		/// Get the width of the text
		/// </summary>
		/// <returns>The width</returns>
		int getWidth()
		{
			if (_base == NULL)
				return 0;

			return _base->transform.w;
		}

		/// <summary>
		/// Get the height of the text
		/// </summary>
		/// <returns>The height</returns>
		int getHeight()
		{
			if (_base == NULL)
				return 0;

			return _base->transform.h;
		}

		/// <summary>
		/// Get the tag of the text
		/// </summary>
		/// <returns>The tag</returns>
		std::string getTag()
		{
			if (_base == NULL)
				return "";
			return _base->tag;
		}

		/// <summary>
		/// Set the tag of the text
		/// </summary>
		/// <param name="tag">The tag</param>
		void setTag(const std::string& tag)
		{
			if (_base == NULL)
				return;
			_base->tag = tag;
		}

		/// <summary>
		/// Get if the text is centered
		/// </summary>
		/// <returns>If the text is centered</returns>
		bool getCentered()
		{
			if (_base == NULL)
				return false;
			return _base->centerLines;
		}

		/// <summary>
		/// Set if the text is centered
		/// </summary>
		/// <param name="centered">If the text is centered</param>
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
				"center", sol::property(&LuaText::getCentered, &LuaText::setCentered),
				sol::base_classes, sol::bases<LuaObject>()
			);
		}
		

	};
}

#endif