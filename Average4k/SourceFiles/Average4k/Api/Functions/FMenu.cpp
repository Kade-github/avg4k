/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FMenu.h"

#include "../../A4kGame.h"
#include "../../Screens/Menu/MainMenu.h"

Average4k::Api::Stubs::LuaMenu* Average4k::Api::Functions::FMenu::CurrentMenu;


void Average4k::Api::Functions::FMenu::SwitchTo(std::string path)
{
	AvgEngine::Game::Instance->SwitchMenu(std::make_shared<Average4k::Screens::Menu::MainMenu>(path));
}
