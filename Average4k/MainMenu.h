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

		std::vector<std::string> listOfCharts;

		int selectedIndex;
		static int selectedDiffIndex;

		void updateList();

		void update(Events::updateEvent event) override;
		void keyDown(SDL_KeyboardEvent event) override;

};

