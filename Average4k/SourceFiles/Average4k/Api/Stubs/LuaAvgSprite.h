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
#include "../../A4kGame.h"

namespace Average4k::Api
{
	class LuaAvgSprite : public AvgEngine::Base::Sprite
	{
	public:
		int id = 0;
		std::string _path;
		LuaAvgTexture* _texture = nullptr;
		LuaAvgRect* _rect = nullptr;

		LuaAvgSprite(int x, int y, std::string path) : AvgEngine::Base::Sprite(x, y)
		{
			_path = path;

			_path = Average4k::A4kGame::gameInstance->skin.GetPath(_path);

			src = { 0,0,1,1 };
			texture = AvgEngine::OpenGL::Texture::createWithImage(_path);
			transform.w = static_cast<float>(texture->width);
			transform.h = static_cast<float>(texture->height);
			iTransform = transform;

			_texture = new LuaAvgTexture(texture->width, texture->height, _path);

			id = Average4k::Api::AvgLuaFile::objects.size();

			_rect = new LuaAvgRect(x, y, this->transform.w, this->transform.h);

			Average4k::Api::AvgLuaFile::objects.push_back(this);
		}

		void draw() override {
			_texture->height = this->texture->height;
			_texture->width = this->texture->width;
			transform = *_rect; // apply custom transformations
			AvgEngine::Base::Sprite::draw();
		}

		LuaAvgRect* getRect() {
			return _rect;
		}

		LuaAvgTexture* getTexture() {
			return _texture;
		}

		void setRect(LuaAvgRect* rect) {
			_rect = rect;
		}

		void setTexture(LuaAvgTexture* texture) {
			_texture = texture;
		}


		static void registerLua(sol::state& state)
		{
			state.new_usertype<LuaAvgSprite>(
				"Sprite", sol::constructors<LuaAvgSprite(int, int, std::string)>(),
				"transform", sol::property(&LuaAvgSprite::getRect, &LuaAvgSprite::setRect),
				"texture", sol::property(&LuaAvgSprite::getTexture, &LuaAvgSprite::setTexture),
				"id", &LuaAvgSprite::id
			);
		}
	};

}

#endif