#pragma once
#include "SMFile.h"
#include "QuaverFile.h"
#include "Menu.h"

enum chartType {
	StepMania = 0,
	Quaver = 1,
	Osu = 2
};

struct song {
	chartType type;
	std::string path;
};

class MainMenu :
    public Menu
{
	public:
		MainMenu();
		static MainMenu* instance;
		static Chart* currentChart;
		static float offset;

		static int selectedDiffIndex;

		std::vector<song> listOfCharts;

		void switchChart(song s);

		int selectedIndex;

		void updateList();

		void update(Events::updateEvent event) override;
		void keyDown(SDL_KeyboardEvent event) override;

};

