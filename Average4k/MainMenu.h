#pragma once
#include "SMFile.h"
#include "QuaverFile.h"
#include "Menu.h"
#include "SettingsMenu.h"
#include "MultiplayerLobbies.h"
#include "SongSelect.h"

class MainMenu :
    public Menu
{
	public:
		MainMenu();

		static MainMenu* instance;
		
		std::vector<Text*> MenuItem;


		int selectedIndex;

		void update(Events::updateEvent event) override;
		void keyDown(SDL_KeyboardEvent event) override;

};

