#pragma once
#include "LuaGameObject.h"
#include "LuaTexture.h"
namespace Average4k::Lua::Base
{
	struct animatedSprite : public gameObject {
	public:
		texture tex = texture("");

		void setCenter(bool center);

		bool getCenter();

		void setFrameSize(int _w, int _h);

		void setFPS(int _fps);
		int getFPS();

		void setFrame(int _frame);
		int getFrame();

		animatedSprite(float x, float y, texture _tex) : gameObject(x, y)
		{
			type = 4;
			tex = _tex;
			transform.w = tex.w;
			transform.h = tex.h;
		}
	};
}