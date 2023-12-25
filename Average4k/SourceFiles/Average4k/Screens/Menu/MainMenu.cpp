/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "../../A4kGame.h"
#include "MainMenu.h"
#include "../../Api/AvgLuaFile.h"
using namespace Average4k::Api;

void Average4k::Screens::Menu::MainMenu::load()
{
	AvgLuaFile lua(Average4k::A4kGame::gameInstance->skin.GetPath("Scripts/MainMenu.lua"));

}

void Average4k::Screens::Menu::MainMenu::draw()
{
	
}
