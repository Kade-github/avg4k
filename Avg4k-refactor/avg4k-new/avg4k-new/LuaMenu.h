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

		MenuLuaFile* file;


		LuaMenu() : AvgEngine::Base::Menu()
		{
			file = NULL;
		}

		LuaMenu(std::string path) : AvgEngine::Base::Menu()
		{
			file = new MenuLuaFile(path);
			luaMenu = true;
		}

		void LuaLoad()
		{
			for (AvgEngine::Base::GameObject* obj : GameObjects)
				delete obj;
			GameObjects.clear();
		}
	};
}