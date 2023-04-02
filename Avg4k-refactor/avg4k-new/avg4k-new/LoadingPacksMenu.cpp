#include "Average4K.h"
#include "StartScreen.h"

bool isLoadingInto = false;

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


	gather.FindPacks("assets/charts/");
}

void LoadingPacksMenu::draw()
{
	progress->text = std::to_string(gather.done) + " loaded.";
	if (gather.finished)
	{
		if (!isLoadingInto)
		{
			for(std::thread& t : gather.threads)
				t.join();
			isLoadingInto = true;
			Average4K::Instance->SwitchMenu(new StartScreen());
		}
	}
	Menu::draw();
}