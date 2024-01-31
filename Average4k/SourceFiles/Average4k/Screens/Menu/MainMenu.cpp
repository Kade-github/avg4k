/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "../../A4kGame.h"
#include "MainMenu.h"
using namespace Average4k::Api;

void Average4k::Screens::Menu::MainMenu::createFile(std::string path, bool reset)
{
	if (reset)
	{
		lua->reset();
		lua->load(Average4k::A4kGame::gameInstance->skin.GetPath(path));
	}
	else
		lua = new AvgLuaFile(Average4k::A4kGame::gameInstance->skin.GetPath(path));

	Average4k::Api::Stubs::LuaMenu::Register(lua->getState());

	Average4k::Api::Stubs::LuaMenu cm(this);

	lua->getState().set("currentMenu", cm);

	lua->getState().set_function("switchTo", [this](std::string path) {
		createFile(path);
	});

	lua->create();
}

void Average4k::Screens::Menu::MainMenu::load()
{
	createFile("Scripts/MainMenu.lua", false);
}

void Average4k::Screens::Menu::MainMenu::draw()
{
	AvgEngine::Base::Menu::draw();
	
}
