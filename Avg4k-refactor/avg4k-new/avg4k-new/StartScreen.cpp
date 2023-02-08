#include "StartScreen.h"

#include "Sprite.h"
#include "FntParser.h"

using namespace AvgEngine;

StartScreen::StartScreen()
{

}

void StartScreen::load()
{
	Sprite* sprite = new Sprite(0, 2, "assets/skinDefaults/Menu/darkmodebg.png");
	sprite->transformRatio = true;
	addObject(sprite);

	tween.CreateTween(&sprite->transform, Render::Rect(0, 0, sprite->transform), 8, Easing::Easing::getEasingFunction("outcubic"), NULL);
}