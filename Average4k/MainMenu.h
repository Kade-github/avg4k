#pragma once
#include "SMFile.h"
#include "QuaverFile.h"
#include "Menu.h"
#include "SettingsMenu.h"
#include "MultiplayerLobbies.h"
#include "SongSelect.h"
#include "AvgSprite.h"
#include "TweenManager.h"

class MainMenu :
    public Menu
{
	public:
		bool started = false;
		AvgSprite* thing;
		AvgSprite* icon;
		AvgSprite* bg;
		std::vector<Tweening::AvgButtonTweenable*> buttons;
		Text* hello;
		Text* bottom;
		SDL_Texture* avatar;
		AvgSprite* border;
		MainMenu();

		static MainMenu* instance;
		
		std::vector<Text*> MenuItem;

		virtual ~MainMenu()
		{
		}

		int selectedIndex = 0;
	
		void onSteam(std::string s) override;
		void update(Events::updateEvent event) override;
		void keyDown(SDL_KeyboardEvent event) override;

};

