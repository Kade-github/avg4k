#pragma once
#include "SMFile.h"
#include "QuaverFile.h"
#include "Menu.h"
#include "SettingsMenu.h"
#include "MultiplayerLobbies.h"
#include "SongSelect.h"
#include "AvgSprite.h"
#include "TweenManager.h"
#include "SoundManager.h"

class MainMenu :
    public Menu
{
	public:
		static bool tweened;
		float lastBop = 0;
		bool tweenDone = false;
		bool started = false;
		float lastBeat = 0;
		AvgSprite* thing;
		AvgSprite* icon;
		AvgSprite* bg;
		std::vector<Tweening::AvgButtonTweenable*> buttons;
		Text* hello;
		Text* bottom;
		SDL_Texture* avatar;
		AvgSprite* border;
		MainMenu();

		
		std::vector<Text*> MenuItem;

		virtual ~MainMenu()
		{
		}

		int selectedIndex = 0;
	
		void onSteam(std::string s) override;
		void update(Events::updateEvent event) override;
		void keyDown(SDL_KeyboardEvent event) override;

};

