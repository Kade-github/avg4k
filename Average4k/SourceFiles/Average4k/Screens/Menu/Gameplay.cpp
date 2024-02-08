/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "Gameplay.h"
#include "MainMenu.h"
#include <AvgEngine/Utils/Paths.h>

void Average4k::Screens::Menu::Gameplay::loadChart()
{
	if (_path.ends_with(L".sm") || _path.ends_with(L".ssc"))
		chart = Average4k::Data::Chart::Providers::StepFile(_path);
}

void Average4k::Screens::Menu::Gameplay::loadBackground()
{
	std::wstring folder = chart.path.substr(0, chart.path.find_last_of(L"/\\"));
	std::wstring bgPath = folder + L"/" + chart.metadata.background;

	std::string strPath = std::string(bgPath.begin(), bgPath.end());

	if (!AvgEngine::Utils::Paths::pathExists(strPath))
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Warning] Background not found.");
		return;
	}

	AvgEngine::Base::Sprite* bg = new AvgEngine::Base::Sprite(0,0, strPath);

	bg->tag = "background";

	// set width and height

	bg->transform.w = AvgEngine::Render::Display::width;
	bg->transform.h = AvgEngine::Render::Display::height;

	bg->transform.a = save->gameplayData.backgroundDim;

	// add
	addObject(bg);
}

void Average4k::Screens::Menu::Gameplay::loadPlayfield()
{
}

void Average4k::Screens::Menu::Gameplay::start()
{
}

void Average4k::Screens::Menu::Gameplay::load()
{
	// Stop all audio
	for (AvgEngine::Audio::Channel* c : AvgEngine::External::BASS::Channels)
	{
		if (c == NULL || c->id == -1)
			continue;
		if (c->isPlaying)
			c->Stop();
		delete c;
	}

	AvgEngine::External::BASS::Channels.clear();

	save = &A4kGame::gameInstance->saveData;
	loadChart();
	if (!chart.isValid)
	{
		AvgEngine::Logging::writeLog("[Gameplay] [Error] Chart is not valid, returning to main menu.");
		AvgEngine::Game::Instance->SwitchMenu(new MainMenu("Scripts/MainMenu.lua"));
		return;
	}

	AvgEngine::Logging::writeLog("[Gameplay] Playing " + std::string(chart.metadata.title.begin(), chart.metadata.title.end()) + " on " + std::string(chart.difficulties[_diff].name.begin(), chart.difficulties[_diff].name.end()));

	loadBackground();

	// event handlers

	eManager->Subscribe(AvgEngine::Events::EventType::Event_KeyPress, [&](AvgEngine::Events::Event e) {
		if (e.data == save->keybindData.keyPause)
			AvgEngine::Game::Instance->SwitchMenu(new MainMenu("Scripts/MainMenu.lua"));
	});
}

void Average4k::Screens::Menu::Gameplay::draw()
{
	AvgEngine::Base::Menu::draw();
}
