/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_SPRITE_H
#define AVG4K_LUA_SPRITE_H

#pragma once

#include "../AvgLuaFile.h"
#include "LuaAvgRect.h"
#include "LuaAvgTexture.h"
#include <AvgEngine/Base/Sprite.h>

namespace Average4k::Api
{
	class LuaAvgSprite : public AvgEngine::Base::Sprite
	{
	public:
		LuaAvgSprite(int x, int y, std::string path) : AvgEngine::Base::Sprite(x, y, path)
		{
			_rect = new LuaAvgRect(x, y, this->transform.w, this->transform.h);
			_texture->path = path;
		}

		void draw() override {
			_texture->height = this->texture->height;
			_texture->width = this->texture->width;
			transform = *_rect; // apply custom transformations
			AvgEngine::Base::Sprite::draw();
		}

		static void registerLua(sol::state& state)
		{
			state.new_usertype<LuaAvgSprite>(
				"Sprite", sol::constructors<LuaAvgSprite(int, int, std::string)>(),
				"transform", sol::property(&LuaAvgSprite::_rect),
				"texture", sol::property(&LuaAvgSprite::_texture)
			);
		}
		LuaAvgTexture* _texture = nullptr;
		LuaAvgRect* _rect = nullptr;
	};

}

#endif