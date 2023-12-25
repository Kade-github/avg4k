/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_TEX_H
#define AVG4K_LUA_TEX_H

#pragma once

#include "../AvgLuaFile.h"
#include <AvgEngine/Render/OpenGL/Texture.h>

namespace Average4k::Api
{
	class LuaAvgTexture
	{
	public:
		LuaAvgTexture(int width, int height, std::string path) : width(width), height(height), path(path)
		{
			
		}

		int width = 0;
		int height = 0;
		std::string path = "";

		static void registerLua(sol::state& state)
		{
			state.new_usertype<LuaAvgTexture>("Texture", sol::constructors<LuaAvgTexture(int, int, std::string)>(), 
				"width", &LuaAvgTexture::width,
											"height", &LuaAvgTexture::height,
											"path", &LuaAvgTexture::path
			);
		}

	};

}

#endif