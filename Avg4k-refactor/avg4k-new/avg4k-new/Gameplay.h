#pragma once
#include "Game.h"
#include "includes.h"
#include "Menu.h"
#include "Container.h"
#include "LuaMenu.h"

class Gameplay : public Average4k::Lua::LuaMenu
{
public:
	Gameplay(std::string path) : Average4k::Lua::LuaMenu(path)
	{
	}

	void Reload() override;

	void load() override;

	void draw() override;
};
