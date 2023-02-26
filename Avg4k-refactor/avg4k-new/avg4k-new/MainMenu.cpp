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

	soloContainer = new Container(Average4K::skin, 0.5, 1.2, "MainMenu/container");
	soloContainer->center = true;
	addObject(soloContainer);

	soloText = new Text(0.33,-0.25, Average4K::skin->GetFontPath(), "AndaleMono.fnt", "solo", 24);
	soloText->characterSpacing = 3;
	soloText->SetColor(255, 255, 255);
	soloText->transformRatio = true;

	addObject(soloText);


	// Tweens

	tween.CreateTween(&soloContainer->transform, Render::Rect(0.5, 0.5, soloContainer->transform), 1.2, Easing::Easing::getEasingFunction("outCubic"), NULL);
	tween.CreateTween(&soloText->transform, Render::Rect(0.33, 0.05, soloText->transform), 1.2, Easing::Easing::getEasingFunction("outCubic"), NULL);
}

void MainMenu::draw()
{

	Menu::draw();
}