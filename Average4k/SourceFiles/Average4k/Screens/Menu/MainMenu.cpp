/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "../../A4kGame.h"
#include "MainMenu.h"
#include "../../Api/AvgLuaFile.h"
#include "../../Api/Stubs/LuaMenu.h"
using namespace Average4k::Api;

void Average4k::Screens::Menu::MainMenu::load()
{
	AvgLuaFile lua(Average4k::A4kGame::gameInstance->skin.GetPath("Scripts/MainMenu.lua"));

	Average4k::Api::Stubs::LuaMenu::Register(lua.getState());

	Average4k::Api::Stubs::LuaMenu cm(this);

	lua.getState().set("currentMenu", cm);

	lua.create();
}

void Average4k::Screens::Menu::MainMenu::draw()
{
	AvgEngine::Base::Menu::draw();
	
}
