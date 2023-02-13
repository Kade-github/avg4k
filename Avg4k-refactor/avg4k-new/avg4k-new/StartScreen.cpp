#include "StartScreen.h"

#include "Average4K.h"
#include "Sprite.h"

using namespace AvgEngine;

StartScreen::StartScreen()
{

}

void StartScreen::load()
{
	Sprite* sprite = new Sprite(0, 2, Average4K::skin->GetTexture("Menu/darkmodebg"));
	sprite->transformRatio = true;
	addObject(sprite);

	tween.CreateTween(&sprite->transform, Render::Rect(0, 0, sprite->transform), 8, Easing::Easing::getEasingFunction("outcubic"), NULL);
	c = Average4k::Audio::RhythmBASSHelper::CreateChannel("menu", Average4K::skin->GetPath("Music/MenuTheme.ogg"));
	c->SetSegments({ Average4k::Utils::SkinUtils::GetMenuThemeTiming() });
	c->Play();
}

void StartScreen::draw()
{
	Logging::writeLog("[StartScreen] Beat: " + std::to_string(static_cast<int>(c->GetBeat())));
	Menu::draw();
}