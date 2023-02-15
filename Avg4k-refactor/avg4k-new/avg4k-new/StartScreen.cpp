#include "StartScreen.h"

#include "Average4K.h"
#include "Sprite.h"

using namespace AvgEngine;

double bSecond = 0;
double bSecond4 = 0;

Easing::Easing::easingFunction outCubic = NULL;

StartScreen::StartScreen()
{

}

void StartScreen::load()
{
	bg = new Sprite(0, 2, Average4K::skin->GetTexture("Menu/darkmodebg"));
	bg->transformRatio = true;
	addObject(bg);

	bump = new Sprite(0.5, 2, Average4K::skin->GetTexture("Menu/avg4k"));
	bump->transformRatio = true;
	bump->center = true;
	addObject(bump);

	logo = new Sprite(0.5, 0.5, Average4K::skin->GetTexture("Start/KadeDevTeam"));
	logo->transformRatio = true;
	logo->center = true;
	logo->transform.a = 0;
	addObject(logo);

	whs = new Sprite(0.5, 0.5, Average4K::skin->GetTexture("Start/MiscLogo"));
	whs->transformRatio = true;
	whs->center = true;
	whs->transform.a = 0;
	addObject(whs);

	c = Average4k::Audio::RhythmBASSHelper::CreateChannel("menu", Average4K::skin->GetPath("Music/MenuTheme.ogg"));
	c->SetSegments({ Average4k::Utils::SkinUtils::GetMenuThemeTiming() });
	bpm = c->timeSegments[0].bpm;
	_eBeat = c->timeSegments[0].id;
	_tBeat = _eBeat / 2;
	c->Play();

	Render::Rect r = logo->transform;
	r.a = 1;
	tween.CreateTween(&logo->transform, r, Average4k::Utils::TimeUtils::ConvertBeatToTime(bpm, _tBeat) / 1000, Easing::Easing::getEasingFunction("outcubic"), NULL);

	bSecond = Average4k::Utils::TimeUtils::ConvertBeatToTime(bpm, 1) / 1000;
	bSecond4 = Average4k::Utils::TimeUtils::ConvertBeatToTime(bpm, 4) / 1000;
	outCubic = Easing::Easing::getEasingFunction("outcubic");
}

void StartScreen::draw()
{
	static int lastBeat = 0;
	static double bumpTime = 0;
	const double beat = c->GetBeat();
	if (std::floor(beat) >= lastBeat + 4)
	{
		bumpTime = glfwGetTime();
		lastBeat = static_cast<int>(beat);
	}
	if (beat >= _tBeat && !startedT)
	{
		startedT = true;
		Render::Rect r = logo->transform;
		r.a = 0;
		tween.CreateTween(&logo->transform, r, bSecond, outCubic, NULL);
		r = whs->transform;
		r.a = 1;
		tween.CreateTween(&whs->transform, r, Average4k::Utils::TimeUtils::ConvertBeatToTime(bpm, _eBeat - _tBeat) / 1000, outCubic, NULL);
	}
	if (beat >= _eBeat && !started)
	{
		Render::Rect r = whs->transform;
		r.a = 0;
		tween.CreateTween(&whs->transform, r, bSecond, Easing::Easing::getEasingFunction("outcubic"), NULL);
		started = true;
		tween.CreateTween(&bg->transform, Render::Rect(0, 0, bg->transform), 1, outCubic, NULL);
		tween.CreateTween(&bump->transform, Render::Rect(0.5, 0.5, bump->transform), 1, outCubic, NULL);
	}
	double t = glfwGetTime();
	double s = std::min((t - bumpTime) / bSecond4, 1.0);
	bump->transform.scale = std::lerp(1.1, 1.0, outCubic(s));

	Menu::draw();
}