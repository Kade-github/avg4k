/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_SPRITE_H
#define AVG4K_LUA_SPRITE_H

#pragma once
#include "../../A4kGame.h"
#include "LuaObject.h"
#include "AvgEngine/Base/Sprite.h"
#include <AvgEngine/Utils/StringTools.h>

#define SOL_NO_EXCEPTIONS 1
#define SOL_USE_LUA_HPP
#include <sol/sol.hpp>

namespace Average4k::Api::Stubs
{

	class LuaSprite : public LuaObject {
		AvgEngine::Base::Sprite* _base;
	public:
		LuaSprite(float x, float y, const std::string& path)
		{
			std::string formatted = path;
			AvgEngine::Utils::StringTools::ToLower(formatted);
			if (formatted.contains("charts"))
				_base = new AvgEngine::Base::Sprite(x, y, path);
			else
				_base = new AvgEngine::Base::Sprite(x, y, A4kGame::gameInstance->skin.GetPath(path));
			_baseObject = _base;
		}

		LuaSprite(AvgEngine::Base::Sprite* sprite)
		{
			_base = sprite;
			_baseObject = _base;
		}

		LuaSprite()
		{
			_base = NULL;
			_baseObject = NULL;
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

		void setWidth(int width)
		{
			if (_base == NULL)
				return;
			_base->transform.w = width;
		}

		int getWidth()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.w;
		}

		void setHeight(int height)
		{
			if (_base == NULL)
				return;
			_base->transform.h = height;
		}

		int getHeight()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.h;
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

		void setColor(float r, float g, float b, float a)
		{
			if (_base == NULL)
				return;
			_base->transform.r = r;
			_base->transform.g = g;
			_base->transform.b = b;
			_base->transform.a = a;
		}

		static void Register(sol::state& state)
		{
			state.new_usertype<LuaSprite>("Sprite",
				sol::constructors<LuaSprite(float, float, const std::string&)>(),
				"x", sol::property(&LuaSprite::getX, &LuaSprite::setX),
				"y", sol::property(&LuaSprite::getY, &LuaSprite::setY),
				"width", sol::property(&LuaSprite::getWidth, &LuaSprite::setWidth),
				"height", sol::property(&LuaSprite::getHeight, &LuaSprite::setHeight),
				"angle", sol::property(&LuaSprite::getAngle, &LuaSprite::setAngle),
				"color", &LuaSprite::setColor,
				sol::base_classes, sol::bases<LuaObject>()
			);
		}
	};
}
#endif