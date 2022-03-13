#pragma once
#include "MainMenu.h"
#include <windows.h>
#include "AvgLua.h"
#include "SPacketAvatarRequestResponse.h"
#include "CPacketRequestAvatar.h"
#include "AvgGroup.h"
#include "FuckinEditor.h"

MainMenu::MainMenu()
{

}

void MainMenu::create() {
	addCamera(Game::mainCamera);

	bg = new AvgSprite(0, 0, Noteskin::getMenuElement(Game::noteskin, "MainMenu/bg.png"));
	bg->create();
	bg->w = 1286;
	bg->h = 726;
	add(bg);

	icon = new AvgSprite(32, 32, Noteskin::getMenuElement(Game::noteskin, "genericAvatar.png"));
	icon->create();
	add(icon);

	thing = new AvgSprite(Game::gameWidth / 2, Game::gameHeight / 2, Noteskin::getMenuElement(Game::noteskin, "MainMenu/avg4k.png"));
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

	MUTATE_END
}

void MainMenu::keyDown(SDL_KeyboardEvent event)
{
	if (!Multiplayer::loggedIn)
		return;

}
