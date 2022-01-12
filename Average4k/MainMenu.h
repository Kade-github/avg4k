#pragma once
#include "SMFile.h"
#include "QuaverFile.h"
#include "Menu.h"
#include "SettingsMenu.h"
#include "MultiplayerLobbies.h"
#include "SongSelect.h"
#include "AvgSprite.h"
#include "AvgButton.h"

class MainMenu :
    public Menu
{
	public:
		AvgSprite* thing;
		AvgSprite* icon;
		std::vector<AvgButton*> buttons;
		Text* hello;
		SDL_Texture* avatar;
		AvgSprite* border;
		MainMenu();

		static MainMenu* instance;
		
		std::vector<Text*> MenuItem;


		int selectedIndex;
	
		void onSteam(std::string s) override;
		void update(Events::updateEvent event) override;
		void keyDown(SDL_KeyboardEvent event) override;

};

