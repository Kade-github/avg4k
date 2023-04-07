#include "Average4K.h"
#include "StartScreen.h"

bool isLoadingInto = false;

float loadingPacks_startTime = 0;

void LoadingPacksMenu::load()
{
	Text* t = new Text(0.5, 0.4, Average4K::skin->GetFontPath(), "ArialBold.fnt", "Loading Packs...", 64);
	t->centerLines = true;
	t->transform.a = 0.5;

	addObject(t);
	t->setRatio(true);

	progress = new Text(0.5, 0.51, Average4K::skin->GetFontPath(), "ArialBold.fnt", "0%", 48);
	progress->centerLines = true;
	progress->transform.a = 0.5;
	addObject(progress);
	progress->setRatio(true);

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
	progress->text = std::to_string(gather.done) + " charts loaded.";
	if (gather.total == gather.done)
	{
		if (!isLoadingInto)
		{
			AvgEngine::Logging::writeLog("[LoadingPacksMenu] Finished loading " + std::to_string(gather.total) + " charts in " + std::to_string(glfwGetTime() - loadingPacks_startTime) + " seconds.");
			isLoadingInto = true;
			Average4K::Instance->SwitchMenu(new StartScreen());
		}
	}
	Menu::draw();
}