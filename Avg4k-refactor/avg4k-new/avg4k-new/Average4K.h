#pragma once
#include "includes.h"
#include "Game.h"
#include "Multiplayer.h"
#include "Paths.h"
#include "Settings.h"
#include "StartScreen.h"
#include "SteamIncludes.h"

class Average4K : public AvgEngine::Game
{
public:
	Average4k::Settings::Settings settings;
	Steam::SteamInterface* steam;
	Steam::SteamWorkshop* workshop;

	Average4K(std::string _t, std::string _v) : Game(_t, _v)
	{
		steam = new Steam::SteamInterface();
		workshop = new Steam::SteamWorkshop();
		settings = Average4k::Settings::Settings(AvgEngine::Utils::Paths::getAppData("Average4K") + "settings.ave");
	}

	void Start()
	{
		SwitchMenu(new StartScreen());

		if (!steam->good)
			return;

		steam->SetPresence(Version + " Alpha | Testing");

		Multiplayer::InitCrypto();

		std::jthread MultiplayerThread = std::jthread([this] {
			Multiplayer::connect(this);
		});
		MultiplayerThread.detach();
	}

	void update() override
	{
		SteamAPI_RunCallbacks();
		Game::update();
	}

	void Destroy()
	{
		steam->destroy();
		workshop->destroy();
	}
};