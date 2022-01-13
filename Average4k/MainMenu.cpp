#pragma once
#include "MainMenu.h"
#include <windows.h>
#include "AvgLua.h"
#include "SPacketAvatarRequestResponse.h"
#include "CPacketRequestAvatar.h"

MainMenu* MainMenu::instance = NULL;

inline bool fileExists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

void changeMenu() {
	switch (MainMenu::instance->selectedIndex)
	{
	case 1:
		Game::instance->transitionToMenu(new MultiplayerLobbies());
		break;
	case 0:
		Game::instance->transitionToMenu(new SongSelect());
		break;
	case 2:
		Game::instance->transitionToMenu(new SettingsMenu());
		break;
	}
}

void callback()
{
	std::cout << "spwan buttons" << std::endl;
	// Colors
	Color c;
	c.r = 255;
	c.g = 136;
	c.b = 0;
	Color filC;
	filC.r = 255;
	filC.g = 255;
	filC.b = 255;
	Color hovered;
	hovered.r = 230;
	hovered.g = 230;
	hovered.b = 230;
	Color font;
	font.r = 255;
	font.g = 0;
	font.b = 142;

	Tweening::AvgButtonTweenable* but = new Tweening::AvgButtonTweenable(250, 500, 250, 45, "solo", 18, "arial", (clickCallback)changeMenu);
	but->create();
	but->text->border = false;
	but->fontColor = font;
	but->borderSize = 2;
	but->borderColor = c;
	but->hoverColor = hovered;
	but->fillColor = filC;
	MainMenu::instance->add(but);
	MainMenu::instance->buttons.push_back(but);
	Tweening::AvgButtonTweenable* but2 = new Tweening::AvgButtonTweenable(532, 500, 250, 45, "multiplayer", 18, "arial", (clickCallback)changeMenu);
	but2->create();
	but2->text->border = false;
	but2->fontColor = font;
	but2->borderSize = 2;
	but2->borderColor = c;
	but2->hoverColor = hovered;
	but2->fillColor = filC;
	MainMenu::instance->add(but2);
	MainMenu::instance->buttons.push_back(but2);
	Tweening::AvgButtonTweenable* but3 = new Tweening::AvgButtonTweenable(832, 500, 250, 45, "settings", 18, "arial", (clickCallback)changeMenu);
	but3->create();
	but3->text->border = false;
	but3->fontColor = font;
	but3->borderSize = 2;
	but3->hoverColor = hovered;
	but3->borderColor = c;
	but3->fillColor = filC;
	MainMenu::instance->add(but3);
	MainMenu::instance->buttons.push_back(but3);
	MainMenu::instance->hello->setText("Hi " + std::string(SteamFriends()->GetPersonaName()));
	MainMenu::instance->bottom->setText("Avg4k indev-" + Game::version);
	Tweening::TweenManager::createNewTween("hello", MainMenu::instance->hello, Tweening::tt_Alpha, 600, 0, 255, NULL, Easing::EaseInSine);
	Tweening::TweenManager::createNewTween("bottom", MainMenu::instance->bottom, Tweening::tt_Alpha, 600, 0, 255, NULL, Easing::EaseInSine);
	int index = 0;
	for (Tweening::AvgButtonTweenable* b : MainMenu::instance->buttons)
	{
		b->alpha = 0;
		Tweening::TweenManager::createNewTween("button" + index, b, Tweening::tt_Alpha, 600, 0, 255, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("buttonY" + index, b, Tweening::tt_Y, 600, b->y + 100, b->y, NULL, Easing::EaseInSine);
		index++;
	}
}

MainMenu::MainMenu()
{
	instance = this;
	bg = new AvgSprite(0, 0, "assets/graphical/menu/mm/bg.png");
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
	thing->alpha = 0;
	thing->create();
	add(thing);

	
	SDL_Rect clip;
	clip.x = 34;
	clip.y = 34;
	clip.w = 43;
	clip.h = 43;
	icon->clipRect = clip;

	hello = new Text(32 + ((icon->w / 2) * 2.6), 36, "Refreshing avatar data...", 16, "arialbd");
	hello->border = false;
	hello->create();
	add(hello);

	bottom = new Text(32 + ((icon->w / 2) * 2.6), 54, "", 16, "arialbd");
	bottom->border = false;
	bottom->create();
	add(bottom);

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
	// I made this math myself
	// it sucks
	// parallax though
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	float mPercX = ((float)mx / (float)(Game::gameWidth / 2));
	float mPercY = ((float)my / (float)(Game::gameHeight / 2));
	bg->setX(-(1.32 * mPercX));
	bg->setY(-(1.32 * mPercY));

	if (!started && Multiplayer::loggedIn)
	{
		started = true;
		Tweening::TweenManager::createNewTween("hello", hello, Tweening::tt_Alpha, 800, 255, 0, NULL, Easing::EaseInSine);
		SDL_Texture* tex = Steam::getAvatar(Multiplayer::currentUserAvatar.c_str());
		if (tex)
		{
			icon->changeOutTexture(tex);
			icon->h = 47;
			icon->w = 47;
		}
		Tweening::TweenManager::createNewTween("menuAlpha", thing, Tweening::tt_Alpha, 1000, 0, 255, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("menu", thing, Tweening::tt_Y, 1000, thing->y + 200, thing->y - 100, (Tweening::tweenCallback)callback, Easing::EaseInSine);
	}

	int index = 0;
	for (Tweening::AvgButtonTweenable* b : MainMenu::instance->buttons)
	{
		if (b->mouse && b->hovered)
			selectedIndex = index;
		index++;
	}
}

void MainMenu::keyDown(SDL_KeyboardEvent event)
{
	if (event.keysym.sym == SDLK_RETURN)
	{
		changeMenu();
	}
	if (event.keysym.sym == SDLK_LEFT)
	{
		buttons[selectedIndex]->selected = false;
		selectedIndex--;
		if (selectedIndex < 0)
			selectedIndex = buttons.size() - 1;
		buttons[selectedIndex]->mouse = false;
		buttons[selectedIndex]->selected = true;
	}

	if (event.keysym.sym == SDLK_RIGHT)
	{
		buttons[selectedIndex]->selected = false;
		selectedIndex++;
		if (selectedIndex > buttons.size() - 1)
			selectedIndex = 0;
		buttons[selectedIndex]->mouse = false;
		buttons[selectedIndex]->selected = true;
	}
}
