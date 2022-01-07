#pragma once
#include "MainMenu.h"
#include <windows.h>
#include "AvgSprite.h"
#include "TweenManager.h"
#include "AvgLua.h"

MainMenu* MainMenu::instance = NULL;

inline bool fileExists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

MainMenu::MainMenu()
{
	instance = this;
	AvgSprite* sprite = new AvgSprite(0, 0, "assets/graphical/menu/bg.png");
	sprite->create();
	add(sprite);

	Text* multiplayer = new Text(Game::gameWidth / 2, (Game::gameHeight / 2), "Multiplayer", 24);
	multiplayer->centerX();
	multiplayer->create();
	add(multiplayer);

	Text* singleplayer = new Text(Game::gameWidth / 2, (Game::gameHeight / 2) + 100, "Singleplayer", 24);
	singleplayer->centerX();
	singleplayer->create();
	add(singleplayer);

	Text* settings = new Text(Game::gameWidth / 2, (Game::gameHeight / 2) + 200, "Settings", 24);
	settings->centerX();
	settings->create();
	add(settings);

	Text* bruh = new Text(Game::gameWidth / 2, (Game::gameHeight / 2) - 155, "Average4k Alpha", 48);
	bruh->centerX();
	bruh->create();
	add(bruh);

	MenuItem.push_back(multiplayer);
	MenuItem.push_back(singleplayer);
	MenuItem.push_back(settings);
	MenuItem.push_back(bruh);
}

void MainMenu::update(Events::updateEvent event)
{
	if (MenuItem.size() == 0)
		return;

	switch (selectedIndex) // does this contradict what we are doing? yes. do I care? no
	{
	case 0:
		
		MenuItem[2]->setText("Settings");
		MenuItem[2]->centerX();
		MenuItem[1]->setText("Singleplayer");
		MenuItem[1]->centerX();
		MenuItem[0]->setText("> Multiplayer");
		MenuItem[0]->centerX();
		break;
	case 1:
		MenuItem[2]->setText("Settings");
		MenuItem[2]->centerX();
		MenuItem[1]->setText("> Singleplayer");
		MenuItem[1]->centerX();
		MenuItem[0]->setText("Multiplayer");
		MenuItem[0]->centerX();
		break;
	case 2:
		MenuItem[2]->setText("> Settings");
		MenuItem[2]->centerX();
		MenuItem[1]->setText("Singleplayer");
		MenuItem[1]->centerX();
		MenuItem[0]->setText("Multiplayer");
		MenuItem[0]->centerX();
		break;
	}
}

void MainMenu::keyDown(SDL_KeyboardEvent event)
{
	if (event.keysym.sym == SDLK_RETURN)
	{
		switch (selectedIndex)
		{
		case 0:
			Game::instance->transitionToMenu(new MultiplayerLobbies());
			break;
		case 1:
			Game::instance->transitionToMenu(new SongSelect());
			break;
		case 2:
			Game::instance->transitionToMenu(new SettingsMenu());
			break;
		}
	}
	if (event.keysym.sym == SDLK_UP)
	{
		selectedIndex--;
		if (selectedIndex < 0)
			selectedIndex = MenuItem.size() - 2;
	}

	if (event.keysym.sym == SDLK_DOWN)
	{
		selectedIndex++;
		if (selectedIndex > MenuItem.size() - 2)
			selectedIndex = 0;
	}
}
