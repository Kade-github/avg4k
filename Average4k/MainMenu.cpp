#pragma once
#include "MainMenu.h"
#include <windows.h>
#include "AvgLua.h"
#include "SPacketAvatarRequestResponse.h"
#include "CPacketRequestAvatar.h"
#include "AvgGroup.h"
#include "FuckinEditor.h"
#include "MainerMenu.h"

float ddddtime = 0;

MainMenu::MainMenu()
{

}

void callback()
{
	Game::instance->switchMenu(new MainerMenu());
}

float mainmenubpm = 155;

void MainMenu::create() {
	

	bg = new AvgSprite(0, 0, Noteskin::getMenuElement(Game::noteskin, "darkmodebg.png"));
	bg->create();
	bg->y = Game::gameHeight;
	bg->w = 1286;
	bg->h = 726;
	add(bg);

	icon = new AvgSprite(32, 32, Noteskin::getMenuElement(Game::noteskin, "genericAvatar.png"));
	icon->create();
	add(icon);

	thing = new AvgSprite(Game::gameWidth / 2, Game::gameHeight / 2, Noteskin::getMenuElement(Game::noteskin, "avg4k.png"));
	thing->x -= thing->w / 2;
	thing->y -= thing->h / 2;
	thing->alpha = 0;
	thing->create();
	add(thing);

	
	Rect clip;
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

	border = new AvgSprite(32, 32, Noteskin::getMenuElement(Game::noteskin,"border.png"));
	border->create();
	add(border);
	created = true;

	Tweening::TweenManager::createNewTween("movingBG", bg, Tweening::tt_Y, 1000, Game::gameHeight, 0, NULL, Easing::EaseOutCubic);
	Tweening::TweenManager::createNewTween("movingLogo", thing, Tweening::tt_Y, 1100, Game::gameHeight, (Game::gameHeight / 2) - (thing->h / 2), NULL, Easing::EaseOutCubic);
	Tweening::TweenManager::createNewTween("alphaLogo", thing, Tweening::tt_Alpha, 1000, 0, 1, NULL, Easing::EaseInSine);
}

void MainMenu::onSteam(std::string s)
{
}

// little helper for this shit
bpmSegment getBPMSegmentFromChart(Chart* chart)
{
	Channel* ch = SoundManager::getChannelByName("prevSong");
	if (ch->id == -1)
	{
		bpmSegment seg = bpmSegment();
		return seg;
	}
	float pos = ch->getPos();
	bpmSegment seg = chart->getSegmentFromTime(pos);
	return seg;
}

void MainMenu::update(Events::updateEvent event)
{
	MUTATE_START

	int mx, my;
	Game::GetMousePos(&mx, &my);
	float mPercX = ((float)mx / (float)(Game::gameWidth / 2));
	float mPercY = ((float)my / (float)(Game::gameHeight / 2));
	bg->setX(-4 - (-(1.5 * mPercX)));
	bg->setY(-4 - (-(1.5 * mPercY)));

	if (SoundManager::getChannelByName("prevSong") != NULL)
	{
			Channel* ch = SoundManager::getChannelByName("prevSong");
			float beat = 0;
			if (SongSelect::currentChart)
			{
				bpmSegment seg = getBPMSegmentFromChart(SongSelect::currentChart);
				ch->bpm = seg.bpm;
				beat = SongSelect::currentChart->getBeatFromTime(ch->getPos() , seg);
			}
			else
			{
				// main menu song
				ch->bpm = mainmenubpm;
				beat = ((ch->getPos()) / 1000) * (ch->bpm / 60);
			}
			if (beat >= lastBeat + 2)
			{
				lastBeat = beat;
				thing->scale = 1.1;
			}

			if (thing->scale > 1.0)
				thing->scale = 1.1 + ((beat - lastBeat) / 1) * (1 - 1.1);

			Game::mainCamera->scale = 1.004 + ((beat - lastBeat) / 1) * (1 - 1.004);
	}

	if (Multiplayer::loggedIn && !started)
	{
		started = true;
		hello->setText("Hi " + std::string(SteamFriends()->GetPersonaName()));
		bottom->setText("Avg4k indev-" + Game::version);
		Tweening::TweenManager::createNewTween("bottom", bottom, Tweening::tt_Alpha, 600, 0, 1, NULL, Easing::EaseInSine);
		Texture* tex = Steam::getAvatar(Multiplayer::currentUserAvatar.c_str());
		if (tex)
		{
			delete icon->tex;
			icon->tex = tex;
			icon->h = 47;
			icon->w = 47;
		}
		Tweening::TweenManager::createNewTween("icon", icon, Tweening::tt_Alpha, 600, 0, 1, NULL, Easing::EaseInSine);
	}


	MUTATE_END
}

void MainMenu::keyDown(SDL_KeyboardEvent event)
{
	if (!Multiplayer::loggedIn)
		return;
	if (event.keysym.sym == SDLK_RETURN && !tweenDone)
	{
		tweenDone = true;
		Tweening::TweenManager::createNewTween("hello", hello, Tweening::tt_Alpha, 600, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("bottom", bottom, Tweening::tt_Alpha, 600, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("border", border, Tweening::tt_Alpha, 600, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("icon", icon, Tweening::tt_Alpha, 600, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("movingLogo", thing, Tweening::tt_Y, 700, (Game::gameHeight / 2) - (thing->h / 2), -(Game::gameHeight / 2) - (thing->h / 2), (Tweening::tweenCallback)callback, Easing::EaseOutCubic);
		Tweening::TweenManager::createNewTween("alphaLogo", thing, Tweening::tt_Alpha, 600, 1, 0, NULL, Easing::EaseInSine);
	}
}
