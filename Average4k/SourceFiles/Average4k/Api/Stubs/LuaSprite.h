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
			_base->texture->dontDelete = false;
			_baseObject = _base;
		}

		LuaSprite(AvgEngine::Base::Sprite* sprite)
		{
			_base = sprite;
			_base->texture->dontDelete = false;
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

		void setR(float r)
		{
			if (_base == NULL)
				return;
			_base->transform.r = r;
		}

		void setG(float g)
		{
			if (_base == NULL)
				return;
			_base->transform.g = g;
		}

		void setB(float b)
		{
			if (_base == NULL)
				return;
			_base->transform.b = b;
		}

		void setA(float a)
		{
			if (_base == NULL)
				return;
			_base->transform.a = a;
		}

		float getR()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.r;
		}

		float getG()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.g;
		}

		float getB()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.b;
		}

		float getA()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.a;
		}

		float getScale()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.scale;
		}

		void setScale(float scale)
		{
			if (_base == NULL)
				return;
			_base->transform.scale = scale;
		}

		void setZIndex(int index)
		{
			if (_base == NULL)
				return;
			_base->zIndex = index;
		}

		int getZIndex()
		{
			if (_base == NULL)
				return 0;
			return _base->zIndex;
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

		static void Register(sol::state& state)
		{
			state.new_usertype<LuaSprite>("Sprite",
				sol::constructors<LuaSprite(float, float, const std::string&)>(),
				"tag", sol::property(&LuaSprite::getTag, &LuaSprite::setTag),
				"x", sol::property(&LuaSprite::getX, &LuaSprite::setX),
				"y", sol::property(&LuaSprite::getY, &LuaSprite::setY),
				"width", sol::property(&LuaSprite::getWidth, &LuaSprite::setWidth),
				"height", sol::property(&LuaSprite::getHeight, &LuaSprite::setHeight),
				"angle", sol::property(&LuaSprite::getAngle, &LuaSprite::setAngle),
				"r", sol::property(&LuaSprite::getR, &LuaSprite::setR),
				"g", sol::property(&LuaSprite::getG, &LuaSprite::setG),
				"b", sol::property(&LuaSprite::getB, &LuaSprite::setB),
				"alpha", sol::property(&LuaSprite::getA, &LuaSprite::setA),
				"scale", sol::property(&LuaSprite::getScale, &LuaSprite::setScale),
				"zIndex", sol::property(&LuaSprite::getZIndex, &LuaSprite::setZIndex),
				sol::base_classes, sol::bases<LuaObject>()
			);
		}
	};
}
#endif