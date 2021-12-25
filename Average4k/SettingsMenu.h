#pragma once
#include "SMFile.h"
#include "QuaverFile.h"
#include "Menu.h"
#include "Game.h"
#include "MainMenu.h"
#include "Text.h"
#include "SaveFile.h"
class SettingsMenu :
	public Menu
{
public:

	struct bruhhh {
		Text* display;
		setting* set;
	};


	std::vector<bruhhh> settings;

	int selectedIndex;

	bool waitingKey = false;

	void refreshList();

	SettingsMenu();


	void update(Events::updateEvent event) override;
	void keyDown(SDL_KeyboardEvent event) override;

};

