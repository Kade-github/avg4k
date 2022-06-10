#pragma once
#include "MainMenu.h"
#include <windows.h>
#include "AvgLua.h"
#include "SPacketAvatarRequestResponse.h"
#include "CPacketRequestAvatar.h"
#include "AvgGroup.h"
#include "FuckinEditor.h"
#include "MainerMenu.h"
#include "CPacketJoinServer.h"
#include "SPacketServerListReply.h"
#include "MultiplayerLobby.h"
#include "Average4k.h"

float ddddtime = 0;

bool MainMenu::first = false;

bool dontAllowInput = false;

MainMenu::MainMenu()
{

}

void callback()
{
	Game::instance->switchMenu(new MainerMenu());
}

float mainmenubpm = 155;

void MainMenu::create() {
	
	MUTATE_START

	bg = new AvgSprite(0, 0, Noteskin::getMenuElement(Game::noteskin, "darkmodebg.png"));
	bg->create();
	bg->y = Game::gameHeight;
	bg->w = 1286;
	bg->h = 726;
	add(bg);


	thing = new AvgSprite(Game::gameWidth / 2, Game::gameHeight / 2, Noteskin::getMenuElement(Game::noteskin, "avg4k.png"));
	thing->x -= thing->w / 2;
	thing->y -= thing->h / 2;
	thing->alpha = 0;
	thing->create();
	add(thing);

	
	icon = new AvgSprite(16, 12, Noteskin::getMenuElement(Game::noteskin, "genericAvatar.png"));
	icon->create();
	add(icon);

	Rect clip;
	clip.x = 18;
	clip.y = 14;
	clip.w = 43;
	clip.h = 43;
	icon->clipRect = clip;

	hello = new Text(16 + ((icon->w / 2) * 2.6), 16, "Refreshing avatar data...", 16, "arialbd");
	hello->border = false;
	hello->create();
	add(hello);

	bottom = new Text(16 + ((icon->w / 2) * 2.6), 30, "", 16, "arialbd");
	bottom->border = false;
	bottom->create();
	add(bottom);

	border = new AvgSprite(16, 12, Noteskin::getMenuElement(Game::noteskin, "border.png"));
	border->create();
	add(border);

	created = true;
	if (!first)
	{
		first = true;
		Tweening::TweenManager::createNewTween("movingBG", bg, Tweening::tt_Y, 1000, Game::gameHeight, 0, NULL, Easing::EaseOutCubic);
	}
	else
	{
		bg->y = 0;
	}
	Tweening::TweenManager::createNewTween("movingLogo", thing, Tweening::tt_Y, 1100, Game::gameHeight, (Game::gameHeight / 2) - (thing->h / 2), NULL, Easing::EaseOutCubic);
	Tweening::TweenManager::createNewTween("alphaLogo", thing, Tweening::tt_Alpha, 1000, 0, 1, NULL, Easing::EaseInSine);
	MUTATE_END
}

void MainMenu::onSteam(std::string s)
{
}

void MainMenu::onPacket(PacketType pt, char* data, int32_t length)
{
	SPacketServerListReply fuck;

	msgpack::unpacked result;

	dontAllowInput = false;

	msgpack::object obj;
	switch (pt)
	{
	case eSPacketJoinServerReply: {
		lobby l;
		l.LobbyID = 0;
		l.LobbyName = "Waiting on refresh";
		l.MaxPlayers = 1;
		player p;
		p.AvatarURL = "";
		p.Name = "You!";
		p.SteamID64 = SteamUser()->GetSteamID().ConvertToUint64();
		l.PlayerList.push_back(p);
		Game::instance->transitionToMenu(new MultiplayerLobby(l, false, false));

		std::cout << "you joined!" << std::endl;
		break;
	}
	}
}

// little helper for this shit
bpmSegment getBPMSegmentFromChart(Chart chart)
{
	VM_START
	Channel* ch = SoundManager::getChannelByName("prevSong");
	if (ch->id == -1)
	{
		bpmSegment seg = bpmSegment();
		return seg;
	}
	float pos = ch->getPos();
	bpmSegment seg = chart.getSegmentFromTime(pos);
	return seg;
	VM_END
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
			if (MainerMenu::currentSelectedSong.meta.songName.size() != 0)
			{
				bpmSegment seg = getBPMSegmentFromChart(MainerMenu::currentSelectedSong);
				ch->bpm = seg.bpm;
				beat = MainerMenu::currentSelectedSong.getBeatFromTime(ch->getPos() , seg);
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

	if (hello->text != "Steam not running" && !Game::instance->isSteam)
	{
		hello->setText("Steam not running");
		bottom->setText("So like, we cant authenticate you lol");
	}

	if (Multiplayer::loggedIn && !started)
	{
		Game::steam->populateSubscribedItems();
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
		Average4k::dumpOutstream();
		char ch[248];
		SteamApps()->GetLaunchCommandLine(ch, 248);
		std::string param = std::string(ch);
		if (param.size() != 0)
		{
			dontAllowInput = true;
			CPacketJoinServer list;
			list.Order = 0;
			list.PacketType = eCPacketJoinServer;
			std::vector<std::string> st = Chart::split(param, ' ');
			std::string sub = st[1];
			list.LobbyID = std::stoul(sub);

			std::cout << "trying to join " << list.LobbyID << " from command line" << std::endl;


			Multiplayer::sendMessage<CPacketJoinServer>(list);
		}
		Average4k::dumpOutstream();
	}


	MUTATE_END
}

void MainMenu::keyDown(SDL_KeyboardEvent event)
{
	MUTATE_START
	if (!Multiplayer::loggedIn || dontAllowInput)
		return;
	if (event.keysym.sym == SDLK_RETURN && !tweenDone && Multiplayer::loggedIn)
	{
		tweenDone = true;
		Tweening::TweenManager::createNewTween("hello", hello, Tweening::tt_Alpha, 600, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("bottom", bottom, Tweening::tt_Alpha, 600, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("border", border, Tweening::tt_Alpha, 600, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("icon", icon, Tweening::tt_Alpha, 600, 1, 0, NULL, Easing::EaseInSine);
		Tweening::TweenManager::createNewTween("movingLogo", thing, Tweening::tt_Y, 700, (Game::gameHeight / 2) - (thing->h / 2), -(Game::gameHeight / 2) - (thing->h / 2), (Tweening::tweenCallback)callback, Easing::EaseOutCubic);
		Tweening::TweenManager::createNewTween("alphaLogo", thing, Tweening::tt_Alpha, 600, 1, 0, NULL, Easing::EaseInSine);
	}
	MUTATE_END
}
