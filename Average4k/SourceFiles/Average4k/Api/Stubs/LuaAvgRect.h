/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_RECT_H
#define AVG4K_LUA_RECT_H

#pragma once

#include "../AvgLuaFile.h"
#include <AvgEngine/Render/Display.h>

namespace Average4k::Api
{
	class LuaAvgRect : public AvgEngine::Render::Rect
	{
	public:
		LuaAvgRect(int x, int y, int w, int h) : AvgEngine::Render::Rect(x, y, w, h)
		{}

		static void registerLua(sol::state& state)
		{
			state.new_usertype<LuaAvgRect>("Rect",
								sol::constructors<LuaAvgRect(int, int, int, int)>(),
								"x", &LuaAvgRect::x,
								"y", &LuaAvgRect::y,
								"w", &LuaAvgRect::w,
								"h", &LuaAvgRect::h,
								"r", &LuaAvgRect::r,
								"g", &LuaAvgRect::g,
								"b", &LuaAvgRect::b,
								"a", &LuaAvgRect::a,
								"angle", &LuaAvgRect::angle
			);
		}
		
	};

}

#endif