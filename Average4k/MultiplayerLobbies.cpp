#include "MultiplayerLobbies.h"
#include "SPacketServerListReply.h"
#include "CPacketServerList.h"
#include "CPacketJoinServer.h"
#include "MultiplayerLobby.h"
#include "CPacketHostServer.h"
#include "MainerMenu.h"

void MultiplayerLobbies::refreshLobbies() {
	VM_START
	if (refreshTimer < 2999)
		return;
	refreshTimer = 0;
	std::cout << "refreshing lobbies" << std::endl;
	CPacketServerList list;
	list.Order = 0;
	list.PacketType = eCPacketServerList;

	Multiplayer::sendMessage<CPacketServerList>(list);
	VM_END
}

MultiplayerLobbies::MultiplayerLobbies()
{
	
}

void MultiplayerLobbies::create()
{
	MUTATE_START;
	
	Game::DiscordUpdatePresence("Browsing lobbies", "Playing Multiplayer", "Average4K", -1, -1, "");
	SteamFriends()->SetRichPresence("gamestatus", "Browsing lobbies");
	SteamFriends()->SetRichPresence("steam_player_group", nullptr);
	SteamFriends()->SetRichPresence("steam_player_group_size", nullptr);
	SteamFriends()->SetRichPresence("status", nullptr);
	SteamFriends()->SetRichPresence("connect", nullptr);
	refreshTimer = 3000;
	if (Multiplayer::loggedIn)
		Game::steam->populateSubscribedItems();

	AvgSprite* sprite = new AvgSprite(0, 0, Noteskin::getMenuElement(Game::noteskin, "darkmodebg.png"));
	sprite->create();
	add(sprite);
	AvgRect* rect = new AvgRect(0, 0, 1280, 720);
	rect->alpha = 0.3;
	rect->create();
	add(rect);
	helpText = new Text(0, 46, "F1 to host a lobby, enter to join (F5 To refresh)", 24, "NotoSans-Regular");
	helpText->create();
	add(helpText);
	refreshLobbies();
	created = true;
	MUTATE_END
}

void MultiplayerLobbies::updateList(std::vector<lobby> lobs)
{
	MUTATE_START

	Lobbies.clear();
	Lobbies = lobs;

	std::cout << "current lobs: " << lobs.size() << std::endl;

	for (Text* t : lobbyTexts)
		removeObj(t);

	lobbyTexts.clear();

	for (bruh t : avatars)
	{
		removeObj(t.avatar);
		delete t.avatar;
	}

	avatars.clear();

	for (int i = 0; i < Lobbies.size(); i++)
	{
		lobby& l = Lobbies[i];
		Text* t = new Text(Game::gameWidth / 2, 100 + (135 * i), l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ")", 24, "NotoSans-Regular");
		t->setX((Game::gameWidth / 2) - (t->surfW / 2));
		t->create();
		for (int p = 0; p < l.PlayerList.size(); p++)
		{
			player& pl = l.PlayerList[p];
			Texture* t = Steam::getAvatar(pl.AvatarURL.c_str());
			bruh b;
			b.p = pl;
			b.avatar = new AvgSprite(0,0,t);
			b.avatar->alpha = 0;
			avatars.push_back(b);
			add(b.avatar);
		}
		lobbyTexts.push_back(t);
		add(t);
	}
	MUTATE_END
}


void MultiplayerLobbies::onPacket(PacketType pt, char* data, int32_t length)
{
	MUTATE_START
	SPacketServerListReply fuck;
	SPacketStatus f;
	msgpack::unpacked result;

	msgpack::object obj;
	switch (pt)
	{
		case eSPacketServerListReply: {
			msgpack::unpack(result, data, length);

			obj = msgpack::object(result.get());

			obj.convert(fuck);

			std::cout << "updating lobbies" << std::endl;

			updateList(fuck.Lobbies);
			break;
		}
		case eSPacketHostServerReply: {
			Game::instance->transitionToMenu(new MultiplayerLobby(Lobbies[selectedIndex], true, false));

			for (bruh t : avatars)
			{
				removeObj(t.avatar);
				delete t.avatar;
			}

			avatars.clear();

			std::cout << "you hosted and joined!" << std::endl;
			break;
		}
		case eSPacketStatus: {
			msgpack::unpack(result, data, length);

			obj = msgpack::object(result.get());

			obj.convert(f);
			
			switch (f.code)
			{
				case 403: {
					std::cout << "not allowed to join that!" << std::endl;
					joiningServer = false;
					break;
				}
				case 404: {
					std::cout << "dawg that lobby aint found" << std::endl;
					joiningServer = false;
					break;
				}
			}
			break;
		}
	}
	MUTATE_END
}

void MultiplayerLobbies::update(Events::updateEvent event)
{
	if (refreshTimer >= 500 && Lobbies.size() == 0)
	{
		refreshLobbies();
	}
	if (refreshTimer < 3000)
		refreshTimer += Game::deltaTime;

	if (lobbyTexts.size() == 0)
		return;


	if (selectedIndex > lobbyTexts.size() - 1)
		selectedIndex = 0;
	if (selectedIndex < 0)
		selectedIndex = lobbyTexts.size() - 1;
	
}

void MultiplayerLobbies::keyDown(SDL_KeyboardEvent event)
{
	MUTATE_START
	if (Lobbies.size() == 0)
	{
		switch (event.keysym.sym)
		{
		case SDLK_ESCAPE:
			Game::instance->transitionToMenu(new MainerMenu());

			for (bruh t : avatars)
				delete t.avatar;

			avatars.clear();

			break;
		case SDLK_F5:
			refreshLobbies();
			break;
		}
	}
	else
	{
		CPacketHostServer host;
		lobby& l = Lobbies[selectedIndex];
		Text* selected = lobbyTexts[selectedIndex];
		switch (event.keysym.sym)
		{
		case SDLK_ESCAPE:
			Game::instance->transitionToMenu(new MainerMenu());

			for (bruh t : avatars)
			{
				removeObj(t.avatar);
				delete t.avatar;
			}

			avatars.clear();

			break;
		case SDLK_F5:
			refreshLobbies();
			break;
		case SDLK_F1:
			host.LobbyName = Multiplayer::username + "'s Lobby";
			host.Order = 0;
			host.PacketType = eCPacketHostServer;

			std::cout << "bruh" << std::endl;

			Multiplayer::sendMessage<CPacketHostServer>(host);
			break;
		case SDLK_RETURN:
			if (joiningServer)
				break;
			joiningServer = true;

			CPacketJoinServer list;
			list.Order = 0;
			list.PacketType = eCPacketJoinServer;
			list.LobbyID = Lobbies[selectedIndex].LobbyID;

			std::cout << "trying to join " << list.LobbyID << std::endl;

			Multiplayer::sendMessage<CPacketJoinServer>(list);
			break;
		case SDLK_UP:
			selected->setText(l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ")");
			selected->setX((Game::gameWidth / 2) - (selected->surfW / 2));
			selectedIndex--;
			break;
		case SDLK_DOWN:
			selected->setText(l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ")");
			selected->setX((Game::gameWidth / 2) - (selected->surfW / 2));
			selectedIndex++;
			break;
		}
	}
	MUTATE_END
}


void MultiplayerLobbies::postUpdate(Events::updateEvent event)
{
	MUTATE_START
	if (lobbyTexts.size() == 0)
		return;

	lobby& l = Lobbies[selectedIndex];
	Text* selected = lobbyTexts[selectedIndex];
	selected->setText("> " + l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ")");
	selected->centerX();

	std::vector<player> playersToShow;

	for (player& p : l.PlayerList)
		playersToShow.push_back(p);

	for (int i = 0; i < avatars.size(); i++)
	{
		bruh& b = avatars[i];

		bool showAvatar = false;

		for (player& p : playersToShow)
		{
			if (p.AvatarURL == b.p.AvatarURL)
			{
				showAvatar = true;
				break;
			}
		}

		AvgSprite* t = b.avatar;
		t->alpha = 0;
		if (!showAvatar)
			continue;

		t->x = selected->x + (52 * i);
		t->y = selected->y + 46;
		t->w = 46;
		t->h = 46;

		t->alpha = 1;

	
		
	}
	MUTATE_END
}