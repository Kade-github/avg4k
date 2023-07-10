#include "Average4K.h"
#include "StartScreen.h"


bool isLoadingInto = false;

float loadingPacks_startTime = 0;

void LoadingPacksMenu::load()
{

	loadingPacks_startTime = glfwGetTime();
	Average4k::External::ConfigReader c = Average4k::External::ConfigReader("assets/songGather.meta");

	int threads = 24;
	if (c.good)
	{
		threads = c.Int("threads");
		AvgEngine::Logging::writeLog("[LoadingPacksMenu] Loading charts with thread count: " + std::to_string(threads));
	}
	else
	{
		AvgEngine::Logging::writeLog("[LoadingPacksMenu] [Warning] Failed to load songGather.meta, using default thread count of 24.");
	}

	gather.FindPacks("assets/charts/", threads);
}

void LoadingPacksMenu::draw()
{
	if (gather.total == gather.done)
	{
		if (!isLoadingInto)
		{
			Average4K::packs = gather.packs;
			gather.packs.clear();
			AvgEngine::Logging::writeLog("[LoadingPacksMenu] Finished loading " + std::to_string(gather.total) + " charts in " + std::to_string(glfwGetTime() - loadingPacks_startTime) + " seconds.");
			isLoadingInto = true;
			static_cast<Average4K*>(Average4K::Instance)->SetSkin(Average4K::settings->Get("Skin").value, false, true);
		}
	}
	Menu::draw();
}