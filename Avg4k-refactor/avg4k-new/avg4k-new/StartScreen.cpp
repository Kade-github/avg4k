#include "StartScreen.h"

#include "Average4K.h"
#include "Sprite.h"

using namespace AvgEngine;

StartScreen::StartScreen()
{

}

void StartScreen::load()
{
	bg = new Sprite(0, 2, Average4K::skin->GetTexture("Menu/darkmodebg"));
	bg->transformRatio = true;
	addObject(bg);

	c = Average4k::Audio::RhythmBASSHelper::CreateChannel("menu", Average4K::skin->GetPath("Music/MenuTheme.ogg"));
	c->SetSegments({ Average4k::Utils::SkinUtils::GetMenuThemeTiming() });
	c->Play();
}

void StartScreen::draw()
{
	const float beat = c->GetBeat();
	const float s = c->timeSegments[0].id;
	if (beat >= s && !started)
	{
		started = true;
		tween.CreateTween(&bg->transform, Render::Rect(0, 0, bg->transform), 1, Easing::Easing::getEasingFunction("outcubic"), NULL);
	}
	Menu::draw();
}