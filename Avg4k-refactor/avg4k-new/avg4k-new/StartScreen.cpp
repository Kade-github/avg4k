#include "StartScreen.h"

#include "Average4K.h"
#include "MainMenu.h"
#include "Sprite.h"
#include "CPacketRequestAvatar.h"
#include "SPacketAvatarRequestResponse.h"
using namespace AvgEngine;

double bSecond = 0;
double bSecond4 = 0;

bool avatar = false;
Average4K* a4k = NULL;
Easing::Easing::easingFunction outCubic = NULL;

StartScreen::StartScreen()
{

}

void StartScreen::load()
{
	a4k = static_cast<Average4K*>(Average4K::Instance);
	eManager->Subscribe(Events::EventType::Event_KeyPress, [&](Events::Event e)
	{
		using namespace Average4k::Utils;
		if (e.data == GLFW_KEY_ENTER && c)
			if (!started)
			{

				c->SetPos(TimeUtils::ConvertBeatToTime(bpm, _eBeat));
			}
			else
			{
				if (Multiplayer::loggedIn)
				{
					tween.CreateTween(&bump->transform, Render::Rect(0.5, 1.2, bump->transform), 1, outCubic, [&]() {
						tween.Clear();
					Average4K::instant = true;
					Average4K::Instance->SwitchMenu(new MainMenu(Average4K::skin->GetLua("main-menu")));
						});
				}
			}
	});

	logo = new Sprite(0.5, 0.5, Average4K::skin->GetTexture("Start/KadeDevTeam"));
	logo->transformRatio = true;
	logo->center = true;
	logo->transform.a = 0;
	logo->transform.scale = Average4K::skin->upscale;
	addObject(logo);

	whs = new Sprite(0.5, 0.5, Average4K::skin->GetTexture("Start/MiscLogo"));
	whs->transformRatio = true;
	whs->center = true;
	whs->transform.a = 0;
	whs->transform.scale = Average4K::skin->upscale;
	addObject(whs);

	bg = new Sprite(0.5, 2, Average4K::skin->GetTexture("Menu/darkmodebg"));
	bg->transformRatio = true;
	bg->center = true;
	bg->transform.scale = Average4K::skin->upscale;
	addObject(bg);

	bump = new Sprite(0.5, 2, Average4K::skin->GetTexture("Menu/avg4k"));
	bump->transformRatio = true;
	bump->center = true;
	bump->transform.scale = Average4K::skin->upscale;
	addObject(bump);

	c = Average4k::Audio::RhythmBASSHelper::CreateChannel("menu", Average4K::skin->GetPath("Music/MenuTheme.ogg"));
	Average4k::External::ConfigReader cf = Average4k::External::ConfigReader(Average4K::skin->GetPath("Music/MenuTheme.meta"));
	bpm = cf.Float("bpm");
	c->SetSegments({ Average4k::Utils::SkinUtils::GetMenuThemeTiming(bpm) });
	_eBeat = cf.Float("beat");
	beatOffset = cf.Float("startOffset");
	_tBeat = (_eBeat - beatOffset) / 2;
	c->Play();

	if (beatOffset < 0)
		beatOffset = 0;

	if (beatOffset != 0)
		c->SetPos(Average4k::Utils::TimeUtils::ConvertBeatToTime(bpm, beatOffset));

	Render::Rect r = logo->transform;
	r.a = 1;
	tween.CreateTween(&logo->transform, r, Average4k::Utils::TimeUtils::ConvertBeatToTime(bpm, _tBeat) / 1000, Easing::Easing::getEasingFunction("outcubic"), NULL);
	bopBeat = cf.Float("bop");
	bSecond = Average4k::Utils::TimeUtils::ConvertBeatToTime(bpm, 1) / 1000;
	bSecond4 = Average4k::Utils::TimeUtils::ConvertBeatToTime(bpm, 4) / 1000;
	outCubic = Easing::Easing::getEasingFunction("outcubic");

	
}

void StartScreen::draw()
{
	if (!avatar && Multiplayer::loggedIn)
	{
		avatar = true;
		Multiplayer::username = a4k->steam->self.nickname;
		AvgEngine::Logging::writeLog("[Steam] User details: " + a4k->steam->self.nickname + ":" + a4k->steam->self.id64);
	}

	static int lastBeat = 0;
	static double bumpTime = 0;
	const double beat = c->GetBeat();
	if (std::floor(beat) > lastBeat + bopBeat)
	{
		bumpTime = glfwGetTime();
		lastBeat = static_cast<int>(beat);
	}
	if (beat >= beatOffset + _tBeat && !startedT)
	{
		startedT = true;
		Render::Rect r = logo->transform;
		r.a = 0;
		tween.CreateTween(&logo->transform, r, bSecond, outCubic, NULL);
		r = whs->transform;
		r.a = 1;
		tween.CreateTween(&whs->transform, r, Average4k::Utils::TimeUtils::ConvertBeatToTime(bpm, _eBeat - (beatOffset + _tBeat)) / 1000, outCubic, NULL);
	}
	if (beat >= _eBeat && !started)
	{
		Render::Rect r = whs->transform;
		r.a = 0;
		tween.CreateTween(&whs->transform, r, bSecond, Easing::Easing::getEasingFunction("outcubic"), NULL);
		started = true;
		tween.CreateTween(&bg->transform, Render::Rect(0.5, 0.5, bg->transform), 1, outCubic, NULL);
		tween.CreateTween(&bump->transform, Render::Rect(0.5, 0.5, bump->transform), 1, outCubic, NULL);
	}
	double t = glfwGetTime();
	double s = std::min((t - bumpTime) / bSecond4, 1.0);
	bump->transform.scale = std::lerp(1.1, 1.0, outCubic(s));

	Menu::draw();
}