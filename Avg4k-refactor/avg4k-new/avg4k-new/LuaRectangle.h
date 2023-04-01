#pragma once
#include "includes.h"
#include "LuaGameObject.h"
#include "Rectangle.h"
namespace Average4k::Lua::Base
{
	struct rectangle : public gameObject
	{
		int getOutline()
		{
			if (base)
			{
				AvgEngine::Base::Rectangle* r = static_cast<AvgEngine::Base::Rectangle*>(base);
				return r->outlinedThickness;
			}
			return 0;
		}

		void setOutline(int outline)
		{
			if (base)
			{
				AvgEngine::Base::Rectangle* r = static_cast<AvgEngine::Base::Rectangle*>(base);
				r->outlinedThickness = outline;
			}
		}

		bool getCentered()
		{
			if (base)
			{
				AvgEngine::Base::Rectangle* r = static_cast<AvgEngine::Base::Rectangle*>(base);
				return r->center;
			}
			return false;
		}

		void setCentered(bool center)
		{
			if (base)
			{
				AvgEngine::Base::Rectangle* r = static_cast<AvgEngine::Base::Rectangle*>(base);
				r->center = center;
			}
		}

		rectangle(float x, float y, float w, float h) : gameObject(x, y)
		{
			transform.w = w;
			transform.h = h;
			type = 3;
		}
	};
}