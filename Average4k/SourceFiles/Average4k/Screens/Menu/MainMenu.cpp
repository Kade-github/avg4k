/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "../../A4kGame.h"
#include "MainMenu.h"

void Average4k::Screens::Menu::MainMenu::load()
{

}

void Average4k::Screens::Menu::MainMenu::draw()
{
	A4kGame::gameInstance->DrawDebugText(32, 32, "This is a test!", 32);
}
