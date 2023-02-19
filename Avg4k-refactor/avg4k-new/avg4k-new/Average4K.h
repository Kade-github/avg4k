#pragma once
#include "includes.h"
#include "Game.h"
#include "Multiplayer.h"
#include "Paths.h"
#include "Settings.h"
#include "StartScreen.h"
#include "SteamIncludes.h"
#include "Notifications.h"
class Average4K : public AvgEngine::Game
{
	double _startTrans = -1;
	bool _out = false;
	Menu* _toSwitch;

public:
	static Average4k::Settings* settings;
	static Average4k::Skin* skin;

	static Average4k::Utils::Notifications* notif;

	static bool instant;

	Steam::SteamInterface* steam;
	Steam::SteamWorkshop* workshop;

	Average4K(std::string _t, std::string _v) : Game(_t, _v)
	{
		steam = new Steam::SteamInterface();
		workshop = new Steam::SteamWorkshop();
		settings = new Average4k::Settings(AvgEngine::Utils::Paths::getAppData("Average4K") + "settings.ave");
		notif = new Average4k::Utils::Notifications(0,0);
	}

	void SetSkin(std::string skinName, bool setValue = true)
	{
		const std::string n = skinName;
		if (setValue)
		{
			Average4k::Setting& s = settings->Get("Skin");
			s.value = n;

		}

		if (skin)
			skin->EmptyCache();
		skin = new Average4k::Skin(settings->Get("Skin").value, "assets/noteskin/");

		AvgEngine::Fnt::Fnt::ClearCache();

		fpsText->SetFont(skin->GetFontPath(), "FuturaBold.fnt");
		alphaText->SetFont(skin->GetFontPath(), "FuturaBold.fnt");

		notif->InitNotifications(skin);

		QueueEvent({ AvgEngine::Events::EventType::Event_ReloadFont,0, {}, skin->GetFontPath() });
	
		SetResolution(settings->Get("Resolution").value);
		fpsText->transform.scale = skin->upscale;
		alphaText->transform.scale = skin->upscale;
		SwitchNoTrans(new StartScreen());
	}

	void SetResolution(std::string res)
	{
		std::vector<std::string> s = AvgEngine::Utils::StringTools::Split(res, "x");
		int w = std::stoi(s[0]);
		int h = std::stoi(s[1]);

		skin->upscale = static_cast<float>(w) / 1280.0f;

		Game::Resize(w, h);
	}

	void Start()
	{
		fpsText = new Text(4, 4, "", "", "FPS: 0", 12);
		fpsText->transform.a = 0.6f;

		alphaText = new Text(4, 4, "", "", "- " + Title + " ALPHA " + Version + " - EVERYTHING IS SUBJECT TO CHANGE -", 14);
		alphaText->transform.a = 0.6f;

		SetSkin(settings->Get("Skin").value, false);
		if (!steam->good)
			return;

		notif->eManager = &eManager;
		notif->Added();

		steam->SetPresence(Version + " Alpha | Testing");
		InitMultiplayer();
	}

	void update() override
	{
		SteamAPI_RunCallbacks();
		if (!CurrentMenu)
			return;

		Game::update();

		// Special text draws'

		using namespace AvgEngine::Render;

		if (alpha)
		{
			alphaText->transform.x += 1;
			alphaText->camera = &CurrentMenu->camera;
			if (alphaText->transform.x > Display::width + alphaText->transform.w)
				alphaText->transform.x = -alphaText->transform.w;
			alphaText->draw();
		}

		fpsText->text = "FPS: " + std::to_string(fps);
		fpsText->camera = &CurrentMenu->camera;
		fpsText->draw();

		// Rect transition
		Rect r = Rect(
			0.0f,0.0f, Display::width, Display::height,
			0.0f,0.0f,0.0f,1.0f);

		// 0.2 second trans time
		if (!_out && _startTrans > 0)
		{
			float t = std::min(static_cast<float>(std::abs(_startTrans - glfwGetTime())) / 0.20f, 1.0f);
			r.a = std::lerp(0, 1, t);
			drawCall c = Camera::FormatDrawCall(0, NULL, NULL, DisplayHelper::RectToVertex(r, { 0,0,1,1 }));

			CurrentMenu->camera.addDrawCall(c);

			if (t >= 1)
			{
				_startTrans = glfwGetTime();
				SwitchNoTrans(_toSwitch);
				_out = true;
			}
		}
		else
		{
			float t = std::min(static_cast<float>(std::abs(_startTrans - glfwGetTime())) / 0.20f, 1.0f);
			r.a = std::lerp(1, 0, t);
			drawCall c = Camera::FormatDrawCall(0, NULL, NULL, DisplayHelper::RectToVertex(r, { 0,0,1,1 }));

			CurrentMenu->camera.addDrawCall(c);
		}
		notif->camera = &CurrentMenu->camera;
		notif->parent = &CurrentMenu->displayRect;
		notif->draw();
	}

	void SwitchNoTrans(Menu* menu)
	{
		Game::SwitchMenu(menu);
	}

	void SwitchMenu(Menu* menu) override
	{
		if (instant)
		{
			instant = false;
			SwitchNoTrans(menu);
			return;
		}
		_toSwitch = menu;
		_startTrans = glfwGetTime();
		_out = false;
	}

	void Destroy()
	{
		steam->destroy();
		workshop->destroy();
	}

	void InitMultiplayer()
	{
		Multiplayer::InitCrypto();

		std::jthread MultiplayerThread = std::jthread([this] {
			Multiplayer::connect(this);
			});
		MultiplayerThread.detach();
	}
};