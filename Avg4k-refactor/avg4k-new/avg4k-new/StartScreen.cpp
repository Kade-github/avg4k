#include "StartScreen.h"

#include "Sprite.h"

StartScreen::StartScreen()
{
	
}

void StartScreen::load()
{
	Sprite* sprite = new Sprite(0, 0, "assets/skinDefaults/Menu/darkmodebg.png");
	addObject(sprite);

}