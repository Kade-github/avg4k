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

#define SOL_NO_EXCEPTIONS 1
#define SOL_USE_LUA_HPP
#include <sol/sol.hpp>

namespace Average4k::Api::Stubs
{

	class LuaSprite : public LuaObject {
		AvgEngine::Base::Sprite* _base;
	public:
		LuaSprite(int x, int y, const std::string& path)
		{
			_base = new AvgEngine::Base::Sprite(x, y, A4kGame::gameInstance->skin.GetPath(path));
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

		void setWidth(int width)
		{
			_base->transform.w = width;
		}

		int getWidth()
		{
			return _base->transform.w;
		}

		void setHeight(int height)
		{
			_base->transform.h = height;
		}

		int getHeight()
		{
			return _base->transform.h;
		}

		void setAngle(float angle)
		{
			_base->transform.angle = angle;
		}

		float getAngle()
		{
			return _base->transform.angle;
		}

		void setColor(int r, int g, int b, int a)
		{
			_base->transform.r = r;
			_base->transform.g = g;
			_base->transform.b = b;
			_base->transform.a = a;
		}

		static void Register(sol::state& state)
		{
			state.new_usertype<LuaSprite>("Sprite",
				sol::constructors<LuaSprite(int, int, const std::string&)>(),
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