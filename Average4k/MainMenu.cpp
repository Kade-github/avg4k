#pragma once
#include "MainMenu.h"
#include <windows.h>
#include "AvgLua.h"
#include "SPacketAvatarRequestResponse.h"
#include "CPacketRequestAvatar.h"
#include "AvgGroup.h"

bool MainMenu::tweened = false;

bool fileExists(const std::string& name) {
	std::ifstream f(name.c_str());
	return f.good();
}

void changeMenu() {
	MainMenu* instance = (MainMenu*)Game::currentMenu;
	for (Tweening::AvgButtonTweenable* b : instance->buttons)
	{
		b->beforeDeath();
		b->die();
	}
	switch (instance->selectedIndex)
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
	VM_START
	MainMenu* instance = (MainMenu*)Game::currentMenu;
	Texture* tex = Steam::getAvatar(Multiplayer::currentUserAvatar.c_str());
	if (tex)
	{
		delete instance->icon->tex;
		instance->icon->tex = tex;
		instance->icon->h = 47;
		instance->icon->w = 47;
	}
	Tweening::TweenManager::createNewTween("border", instance->border, Tweening::tt_Alpha, 600, 0, 255, NULL, Easing::EaseInSine);
	Tweening::TweenManager::createNewTween("icon", instance->icon, Tweening::tt_Alpha, 600, 0, 1, NULL, Easing::EaseInSine);
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
	instance->add(but);
	instance->buttons.push_back(but);
	Tweening::AvgButtonTweenable* but2 = new Tweening::AvgButtonTweenable(532, 500, 250, 45, "multiplayer", 18, "arial", (clickCallback)changeMenu);
	but2->create();
	but2->text->border = false;
	but2->fontColor = font;
	but2->borderSize = 2;
	but2->borderColor = c;
	but2->hoverColor = hovered;
	but2->fillColor = filC;
	instance->add(but2);
	instance->buttons.push_back(but2);
	Tweening::AvgButtonTweenable* but3 = new Tweening::AvgButtonTweenable(832, 500, 250, 45, "settings", 18, "arial", (clickCallback)changeMenu);
	but3->create();
	but3->text->border = false;
	but3->fontColor = font;
	but3->borderSize = 2;
	but3->hoverColor = hovered;
	but3->borderColor = c;
	but3->fillColor = filC;
	instance->add(but3);
	instance->buttons.push_back(but3);
	instance->hello->setText("Hi " + std::string(SteamFriends()->GetPersonaName()));
	instance->bottom->setText("Avg4k indev-" + Game::version);
	Tweening::TweenManager::createNewTween("hello", instance->hello, Tweening::tt_Alpha, 600, 0, 1, NULL, Easing::EaseInSine);
	Tweening::TweenManager::createNewTween("bottom", instance->bottom, Tweening::tt_Alpha, 600, 0, 1, NULL, Easing::EaseInSine);
	int index = 0;
	instance->tweenDone = true;
	for (Tweening::AvgButtonTweenable* b : instance->buttons)
	{
		b->alpha = 0;
		Tweening::TweenManager::createNewTween("button" + index, b, Tweening::tt_Alpha, 600, 0, 1, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("buttonY" + index, b, Tweening::tt_Y, 600, b->y + 100, b->y, NULL, Easing::EaseInSine);
		index++;
	}
	VM_END
}

MainMenu::MainMenu()
{

}

void MainMenu::create() {
	addCamera(Game::mainCamera);

	bg = new AvgSprite(0, 0, "assets/graphical/menu/mm/bg.png");
	bg->create();
	bg->w = 1286;
	bg->h = 726;
	add(bg);

	icon = new AvgSprite(32, 32, "assets/graphical/menu/genericAvatar.png");
	icon->create();
	add(icon);

	thing = new AvgSprite(Game::gameWidth / 2, Game::gameHeight / 2, "assets/graphical/menu/mm/avg4k.png");
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
	//icon->clipRect = clip;

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
}

void MainMenu::onSteam(std::string s)
{
}

void call() {
	MainMenu* instance = (MainMenu*)Game::currentMenu;
	Channel* ch = SoundManager::getChannelByName("prevSong");
	Tweening::TweenManager::createNewTween("thingBeat",instance->thing, Tweening::tt_scale, 19750 / ch->bpm, 1.1, 1, NULL, Easing::EaseInSine);
}


void MainMenu::update(Events::updateEvent event)
{
	MUTATE_START
	// I made this math myself
	// it sucks
	// parallax though
	int mx, my;
	Game::GetMousePos(&mx, &my);
	float mPercX = ((float)mx / (float)(Game::gameWidth / 2));
	float mPercY = ((float)my / (float)(Game::gameHeight / 2));
	bg->setX(-4 - (-(1.5 * mPercX)));
	bg->setY(-4 - (-(1.5 * mPercY)));

	if (SoundManager::getChannelByName("prevSong") != NULL)
	{
		if (tweenDone)
		{
			Channel* ch = SoundManager::getChannelByName("prevSong");
			bpmSegment seg = ch->getBPMSegmentFromChart(SongSelect::currentChart);
			ch->bpm = seg.bpm;
			float beat = SongSelect::currentChart->getBeatFromTime(ch->getPos(), seg);
			if (beat >= lastBeat + 1)
			{
				lastBeat = beat;
				thing->scale = 1.1;
			}
			
			if (thing->scale > 1.0)
				thing->scale -= (Game::deltaTime * ((ch->bpm / 60) / 1000)) * 0.3;

			
		}
	}

	if (!Multiplayer::loggedIn && !started)
		tweened = false;

	if (tweened && !started)
	{
		icon->alpha = 0;
		border->alpha = 0;
		hello->alpha = 0;
		thing->setY(thing->y - 100);
		thing->alpha = 1;
		started = true;
		callback();
	}


	if (!started && Multiplayer::loggedIn)
	{
		tweened = true;
		started = true;
		Tweening::TweenManager::createNewTween("hello", hello, Tweening::tt_Alpha, 800, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("border", border, Tweening::tt_Alpha, 800, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("icon", icon, Tweening::tt_Alpha, 800, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("menuAlpha", thing, Tweening::tt_Alpha, 1000, 0, 1, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("menu", thing, Tweening::tt_Y, 1000, thing->y + 200, thing->y - 100, (Tweening::tweenCallback)callback, Easing::EaseInSine);
	}

	int index = 0;
	for (Tweening::AvgButtonTweenable* b : buttons)
	{
		if (b->mouse && b->hovered)
			selectedIndex = index;
		index++;
	}
	MUTATE_END
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
