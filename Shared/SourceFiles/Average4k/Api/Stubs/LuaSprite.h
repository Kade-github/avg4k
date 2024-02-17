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
		/// <summary>
		/// Create a new sprite
		/// </summary>
		/// <param name="x">The x position</param>
		/// <param name="y">The y position</param>
		/// <param name="path">The path to the sprite</param>
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

		/// <summary>
		/// Set the x position of the sprite
		/// </summary>
		/// <param name="x">The x position</param>
		void setX(float x)
		{
			if (_base == NULL)
				return;
			_base->transform.x = x;
		}

		/// <summary>
		/// Get the x position of the sprite
		/// </summary>
		/// <returns>The x position</returns>
		float getX()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.x;
		}

		/// <summary>
		/// Set the y position of the sprite
		/// </summary>
		/// <param name="y">The y position</param>
		void setY(float y)
		{
			if (_base == NULL)
				return;
			_base->transform.y = y;
		}

		/// <summary>
		/// Get the y position of the sprite
		/// </summary>
		/// <returns>The y position</returns>
		float getY()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.y;
		}

		/// <summary>
		/// Set the width of the sprite
		/// </summary>
		/// <param name="width">The width</param>
		void setWidth(int width)
		{
			if (_base == NULL)
				return;
			_base->transform.w = width;
		}

		/// <summary>
		/// Get the width of the sprite
		/// </summary>
		/// <returns>The width</returns>
		int getWidth()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.w;
		}

		/// <summary>
		/// Set the height of the sprite
		/// </summary>
		/// <param name="height">The height</param>
		void setHeight(int height)
		{
			if (_base == NULL)
				return;
			_base->transform.h = height;
		}

		/// <summary>
		/// Get the height of the sprite
		/// </summary>
		/// <returns>The height</returns>
		int getHeight()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.h;
		}

		/// <summary>
		/// Set the angle of the sprite
		/// </summary>
		/// <param name="angle">The angle</param>
		void setAngle(float angle)
		{
			if (_base == NULL)
				return;
			_base->transform.angle = angle;
		}

		/// <summary>
		/// Get the angle of the sprite
		/// </summary>
		/// <returns>The angle</returns>
		float getAngle()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.angle;
		}

		/// <summary>
		/// Set the red color of the sprite
		/// </summary>
		/// <param name="r">The red color</param>
		void setR(float r)
		{
			if (_base == NULL)
				return;
			_base->transform.r = r;
		}

		/// <summary>
		/// Set the green color of the sprite
		/// </summary>
		/// <param name="g">The green color</param>
		void setG(float g)
		{
			if (_base == NULL)
				return;
			_base->transform.g = g;
		}

		/// <summary>
		/// Set the blue color of the sprite
		/// </summary>
		/// <param name="b">The blue color</param>
		void setB(float b)
		{
			if (_base == NULL)
				return;
			_base->transform.b = b;
		}

		/// <summary>
		/// Set the alpha of the sprite
		/// </summary>
		/// <param name="a">The alpha</param>
		void setA(float a)
		{
			if (_base == NULL)
				return;
			_base->transform.a = a;
		}

		/// <summary>
		/// Get the red color of the sprite
		/// </summary>
		/// <returns>The red color</returns>
		float getR()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.r;
		}

		/// <summary>
		/// Get the green color of the sprite
		/// </summary>
		/// <returns>The green color</returns>
		float getG()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.g;
		}

		/// <summary>
		/// Get the blue color of the sprite
		/// </summary>
		/// <returns>The blue color</returns>
		float getB()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.b;
		}

		/// <summary>
		/// Get the alpha of the sprite
		/// </summary>
		/// <returns>The alpha</returns>
		float getA()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.a;
		}

		/// <summary>
		/// Get the scale of the sprite
		/// </summary>
		/// <returns>The scale</returns>
		float getScale()
		{
			if (_base == NULL)
				return 0;
			return _base->transform.scale;
		}

		/// <summary>
		/// Set the scale of the sprite
		/// </summary>
		/// <param name="scale">The scale</param>
		void setScale(float scale)
		{
			if (_base == NULL)
				return;
			_base->transform.scale = scale;
		}

		/// <summary>
		/// Set the z index of the sprite
		/// </summary>
		/// <param name="index">The z index</param>
		void setZIndex(int index)
		{
			if (_base == NULL)
				return;
			_base->zIndex = index;
		}

		/// <summary>
		/// Get the z index of the sprite
		/// </summary>
		/// <returns>The z index</returns>
		int getZIndex()
		{
			if (_base == NULL)
				return 0;
			return _base->zIndex;
		}

		/// <summary>
		/// Get the tag of the sprite
		/// </summary>
		/// <returns>The tag</returns>
		std::string getTag()
		{
			if (_base == NULL)
				return "";
			return _base->tag;
		}

		/// <summary>
		/// Set the tag of the sprite
		/// </summary>
		/// <param name="tag">The tag</param>
		void setTag(const std::string& tag)
		{
			if (_base == NULL)
				return;
			_base->tag = tag;
		}

		/// <summary>
		/// Set the source rectangle of the sprite
		/// </summary>
		/// <param name="x">The x position</param>
		void setSrcRec(float x, float y, float w, float h)
		{
			if (_base == NULL)
				return;
			_base->src.x = x;
			_base->src.y = y;
			_base->src.w = w;
			_base->src.h = h;
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
				"setSrcRec", &LuaSprite::setSrcRec,
				sol::base_classes, sol::bases<LuaObject>()
			);
		}
	};
}
#endif