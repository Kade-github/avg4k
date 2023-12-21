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

		void recalculateRatio();

		bool getClipAll()
		{
			if (base)
				return base->clipAll;
			return true;
		}

		void setClipAll(bool clip)
		{
			if (base)
				base->clipAll = clip;
		}

		sprite(float x, float y, texture _tex) : gameObject(x, y)
		{
			
			type = 1;
			tex = _tex;
			transform.w = tex.w;
			transform.h = tex.h;
		}
	};
}