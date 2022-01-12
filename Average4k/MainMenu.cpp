#pragma once
#include "MainMenu.h"
#include <windows.h>
#include "TweenManager.h"
#include "AvgLua.h"
#include "SPacketAvatarRequestResponse.h"
#include "CPacketRequestAvatar.h"

MainMenu* MainMenu::instance = NULL;

inline bool fileExists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

void callback()
{
	
}

MainMenu::MainMenu()
{
	instance = this;
	AvgSprite* bg = new AvgSprite(0, 0, "assets/graphical/menu/mm/bg.png");
	bg->create();
	add(bg);

	icon = new AvgSprite(32, 32, "assets/graphical/menu/genericAvatar.png");
	icon->create();
	add(icon);

	thing = new AvgSprite(Game::gameWidth / 2, Game::gameHeight / 2, "assets/graphical/menu/mm/avg4k.png");
	thing->w = thing->w * 0.18;
	thing->h = thing->h * 0.18;
	thing->x -= thing->w / 2;
	thing->y -= thing->h / 2;
	thing->create();
	add(thing);

	AvgButton* but = new AvgButton(100, 100, 100, 24, "hello!");
	but->create();
	Color c;
	c.r = 255;
	c.g = 136;
	c.b = 0;
	Color filC;
	filC.r = 255;
	filC.g = 255;
	filC.b = 255;
	but->borderColor = c;
	but->fillColor = filC;
	add(but);
	
	SDL_Rect clip;
	clip.x = 34;
	clip.y = 34;
	clip.w = 42;
	clip.h = 42;
	icon->clipRect = clip;

	hello = new Text(32 + ((icon->w / 2) * 2.6), 36, "Refreshing avatar data...", 16, "arialbd");
	hello->create();
	add(hello);
	std::cout << "setting font" << std::endl;
	hello->setFont("arialbd");

	border = new AvgSprite(32, 32, "assets/graphical/menu/border.png");
	border->create();
	add(border);

	/*Text* multiplayer = new Text(Game::gameWidth / 2, (Game::gameHeight / 2), "Multiplayer", 24);
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
	MenuItem.push_back(bruh);*/

}

void MainMenu::onSteam(std::string s)
{
}

void MainMenu::update(Events::updateEvent event)
{
	if (!hello->text.starts_with("Hi") && Multiplayer::loggedIn)
	{
		hello->setText("Hi " + std::string(SteamFriends()->GetPersonaName()));
		SDL_Texture* tex = Steam::getAvatar(Multiplayer::currentUserAvatar.c_str());
		if (tex)
		{
			icon->changeOutTexture(tex);
			icon->h = 47;
			icon->w = 47;
		}
		Tweening::TweenManager::createNewTween("menu", thing, Tweening::tt_Y, 750, thing->y, thing->y - 100, (Tweening::tweenCallback)callback, Easing::EaseInSine);
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
	if (event.keysym.sym == SDLK_LEFT)
	{
		selectedIndex--;
		if (selectedIndex < 0)
			selectedIndex = MenuItem.size() - 2;
	}

	if (event.keysym.sym == SDLK_RIGHT)
	{
		selectedIndex++;
		if (selectedIndex > MenuItem.size() - 2)
			selectedIndex = 0;
	}
}
