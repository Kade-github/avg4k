/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "A4kGame.h"

#include <AvgEngine/External/Image/stbi.h>
#include "Data/Chart/ChartFinder.h"
#include "Console/CmdHandler.h"
#include <AvgEngine/External/Bass/BASS.h>

Average4k::A4kGame* Average4k::A4kGame::gameInstance = nullptr;

Average4k::A4kGame::A4kGame(std::string _t, std::string _v, int w, int h) : Game(_t, _v, w, h)
{
	fpsCap = 144;
	gameInstance = this;
}

void Average4k::A4kGame::Destroy()
{
	saveData.Save("Assets/Save/Save.avg");

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
}

void Average4k::A4kGame::Start()
{
	// Set window icon

	GLFWimage images[4];

	images[0].pixels = AvgEngine::External::stbi_h::stbi_load_file_data("Assets/Icon/A4kIcon32.png", &images[0].width, &images[0].height);
	images[1].pixels = AvgEngine::External::stbi_h::stbi_load_file_data("Assets/Icon/A4kIcon64.png", &images[3].width, &images[3].height);
	images[2].pixels = AvgEngine::External::stbi_h::stbi_load_file_data("Assets/Icon/A4kIcon128.png", &images[2].width, &images[2].height);
	images[3].pixels = AvgEngine::External::stbi_h::stbi_load_file_data("Assets/Icon/A4kIcon256.png", &images[1].width, &images[1].height);
	glfwSetWindowIcon(Window, 1, images);

	AvgEngine::Logging::writeLog("[Average4k] Set window icon!");

	// Get save file

	saveData = Data::SaveData::Load("Assets/Save/Save.avg");

	skin = Skin(saveData.skinData);

	console.handler = new Average4k::Console::CmdHandler();

	// Load charts

	Data::ChartFinder::FindCharts("Charts");

}

void Average4k::A4kGame::update()
{
	if (Average4k::Data::ChartFinder::pack_pool.get_tasks_queued() || Average4k::Data::ChartFinder::pack_pool.get_tasks_running() > 0 || Average4k::Data::ChartFinder::startedSearching)
	{
		DrawOutlinedDebugText(200, 100, "please hold... im loading packs", 84);

		DrawOutlinedDebugText(200, 200, "Packs queued to be loaded: " + std::to_string(Average4k::Data::ChartFinder::pack_pool.get_tasks_queued()), 64);

		DrawOutlinedDebugText(200, 300, "Packs currently loading: " + std::to_string(Average4k::Data::ChartFinder::pack_pool.get_tasks_running()), 64);

		DrawOutlinedDebugText(200, 400, "Packs loaded: " + std::to_string(Average4k::Data::ChartFinder::Packs.size()), 64);

		if (CurrentMenu != NULL)
			CurrentMenu->cameraDraw();
	}
	else
		Game::update();

}
