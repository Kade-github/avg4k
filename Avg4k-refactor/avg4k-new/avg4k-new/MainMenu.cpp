#include "MainMenu.h"
#include "StartScreen.h"
#include "Average4K.h"

using namespace AvgEngine;

MainMenu::MainMenu()
{

}

void MainMenu::load()
{
	Sprite* bg = new Sprite(0.5, 0.5, Average4K::skin->GetTexture("Menu/darkmodebg"));
	bg->transformRatio = true;
	bg->center = true;
	bg->transform.scale = Average4K::skin->upscale;
	addObject(bg);
}

void MainMenu::draw()
{

	Menu::draw();
}