#include "StartScreen.h"

#include "Sprite.h"

StartScreen::StartScreen()
{
	
}

void StartScreen::load()
{
	AvgEngine::OpenGL::Texture* bg = AvgEngine::OpenGL::Texture::createWithImage("assets/skinDefaults/Menu/darkmodebg.png");
	Sprite* sprite = new Sprite(0, 0, bg);
	addObject(sprite);
	AvgEngine::Render::Rect nR = sprite->transform;
	nR.x = 100;

	tween.CreateTween(&sprite->transform, nR, 2, AvgEngine::Easing::Easing::getEasingFunction("inoutcubic"), []()
	{
		AvgEngine::Logging::writeLog("done");
	});
}