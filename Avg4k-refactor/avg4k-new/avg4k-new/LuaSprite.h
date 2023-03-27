#pragma once
#include "LuaGameObject.h"
#include "LuaTexture.h"
namespace Average4k::Lua::Base
{
	struct sprite : public gameObject {
	public:
		texture tex = texture("");

		void setCenter(bool center);

		bool getCenter();

		sprite(float x, float y, texture _tex) : gameObject(x, y)
		{
			type = 1;
			tex = _tex;
			transform.w = tex.w;
			transform.h = tex.h;
		}
	};
}