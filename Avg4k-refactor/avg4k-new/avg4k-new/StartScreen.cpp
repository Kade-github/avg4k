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

	Text* t = new Text(5, 5, "assets/graphical/fonts/", "FuturaBold.fnt", "This is a stress test of this font renderer aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Very long text with a lot of words and letters like x y and z. With $pecial characters and other stuff @ average4k hq", 64);
	t->wrap = true;
	//addObject(t);
}