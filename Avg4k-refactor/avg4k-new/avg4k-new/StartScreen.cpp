#include "StartScreen.h"

#include "Sprite.h"
#include "FntParser.h"

StartScreen::StartScreen()
{

}

void StartScreen::load()
{
	AvgEngine::OpenGL::Texture* bg = AvgEngine::OpenGL::Texture::createWithImage("assets/skinDefaults/Menu/darkmodebg.png");
	Sprite* sprite = new Sprite(0, 0, bg);
	addObject(sprite);
}