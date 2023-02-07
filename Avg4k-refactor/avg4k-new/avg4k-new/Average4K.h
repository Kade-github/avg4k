#pragma once
#include "includes.h"
#include "Game.h"
#include "StartScreen.h"
#include "SteamIncludes.h"

class Average4K
{
public:
	static Steam::SteamInterface* steam;
	static Steam::SteamWorkshop* workshop;

	static void Start(AvgEngine::Game* g)
	{
		g->SwitchMenu(new StartScreen());
		steam = new Steam::SteamInterface();
		workshop = new Steam::SteamWorkshop();

		if (!steam->good)
			return;

		steam->SetPresence(g->Version + " Alpha | Testing");
	}

	static void Destroy()
	{
		steam->destroy();
		workshop->destroy();
	}
};

Steam::SteamInterface* Average4K::steam = NULL;
Steam::SteamWorkshop* Average4K::workshop = NULL;