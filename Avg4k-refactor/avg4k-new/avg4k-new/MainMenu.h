#pragma once
#include "Game.h"
#include "includes.h"
#include "Menu.h"
#include "LuaMenu.h"

class MainMenu : public Average4k::Lua::LuaMenu
{
public:
	MainMenu(std::string path) : Average4k::Lua::LuaMenu(path)
	{
	}

	void Reload() override;

	void load() override;

	void draw() override;
};

