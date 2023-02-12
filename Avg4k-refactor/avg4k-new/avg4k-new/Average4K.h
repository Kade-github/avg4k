#pragma once
#include "includes.h"
#include "Game.h"
#include "Multiplayer.h"
#include "Paths.h"
#include "Settings.h"
#include "StartScreen.h"
#include "SteamIncludes.h"
#include "Skin.h"

class Average4K : public AvgEngine::Game
{
	double _startTrans = 0;

public:
	static Average4k::Settings::Settings* settings;
	static Average4k::Skin::Skin* skin;

	Steam::SteamInterface* steam;
	Steam::SteamWorkshop* workshop;

	Average4K(std::string _t, std::string _v) : Game(_t, _v)
	{
		steam = new Steam::SteamInterface();
		workshop = new Steam::SteamWorkshop();
		settings = new Average4k::Settings::Settings(AvgEngine::Utils::Paths::getAppData("Average4K") + "settings.ave");
	}

	void SetSkin(std::string skinName, bool setValue = true)
	{
		const std::string n = skinName;
		if (setValue)
		{
			Average4k::Settings::Setting s = settings->Get("Skin");
			s.value = n;
			settings->Set(s);
		}

		if (skin)
			skin->EmptyCache();
		skin = new Average4k::Skin::Skin(settings->Get("Skin").value, "assets/noteskin/");
		SwitchMenu(new StartScreen());
	}

	void Start()
	{
		SetSkin(settings->Get("Skin").value, false);

		if (!steam->good)
			return;

		steam->SetPresence(Version + " Alpha | Testing");
		InitMultiplayer();
	}

	void update() override
	{
		SteamAPI_RunCallbacks();
		Game::update();

		if (!CurrentMenu)
			return;

		using namespace AvgEngine::Render;
		// Rect transition
		Rect r = Rect(
			0.0f,0.0f, Display::width, Display::height,
			0.0f,0.0f,0.0f,1.0f);

		// 1 second trans time
		float t = std::min(static_cast<float>(std::abs(_startTrans - glfwGetTime())) / 1.0f, 1.0f);
		r.a = std::lerp(1, 0, t);
		drawCall c =  CurrentMenu->camera.FormatDrawCall(0, NULL, NULL, DisplayHelper::RectToVertex(r, { 0,0,1,1 }));
		CurrentMenu->camera.addDrawCall(c);
	}

	void SwitchMenu(Menu* menu) override
	{
		Game::SwitchMenu(menu);
		_startTrans = glfwGetTime();
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