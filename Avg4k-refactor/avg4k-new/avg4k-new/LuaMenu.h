#pragma once
#include "includes.h"
#include "Menu.h"
#include "MenuLuaFile.h"
namespace Average4k::Lua
{
	class LuaMenu : public AvgEngine::Base::Menu
	{
	public:
		bool luaMenu = false;

		MenuLuaFile file;

		LuaMenu() : AvgEngine::Base::Menu()
		{

		}
	};
}