#pragma once
#include "includes.h"
#include "LuaFile.h"
namespace Average4k::Lua
{
	class MenuLuaFile : public LuaFile
	{
	public:
		MenuLuaFile(std::string path) : LuaFile(path)
		{

		}

		void Load() override
		{

		}
	};
}