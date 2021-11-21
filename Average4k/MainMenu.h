#pragma once
#include "SMFile.h"
#include "Menu.h"
class MainMenu :
    public Menu
{
	public:
		MainMenu();
		static MainMenu* instance;
		static Chart* currentChart;
		static float offset;

		static int selectedDiffIndex;

		std::vector<std::string> listOfCharts;

		int selectedIndex;

		void updateList();

		void update(Events::updateEvent event) override;
		void keyDown(SDL_KeyboardEvent event) override;

};

