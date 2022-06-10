#include "MultiplayerLobby.h"
#include "AvgSprite.h"
#include "MainerMenu.h"
#include "SPacketFuckYou.h"

bool MultiplayerLobby::inLobby = false;
lobby MultiplayerLobby::CurrentLobby;
bool MultiplayerLobby::isHost = false;
std::string MultiplayerLobby::hostSteamId;

AvgGroup* playerList;

bool isAwaitingPack = false;

int scrollAdd = 0;
int scroll = 0;

void MultiplayerLobby::refreshLobby(lobby l)
{
	STR_ENCRYPT_START
	CurrentLobby = l;
	inLobby = true;

	for (person& p : people)
	{
		playerList->removeObj(p.avatar);
		playerList->removeObj(p.display);
	}

	scrollAdd = 0;
	people.clear();

	for (int i = 0; i < l.PlayerList.size(); i++)
	{
		player& p = l.PlayerList[i];
		if (i == 0)
			hostSteamId = p.SteamID64;
		person per;
		per.display = new Text(82, 192 + (46 * i), p.Name, 24, "NotoSans-Regular");
		per.display->create();
		Texture* t = Steam::getAvatar(p.AvatarURL.c_str());
		AvgSprite* spr = new AvgSprite(0, 0, t);
		if (t)
			per.avatar = spr;
		else
			per.avatar = NULL;
		spr->w = 46;
		spr->h = 46;

		if (per.display->y > 720)
			scrollAdd += 46;

		playerList->add(per.display);
		playerList->add(spr);
		people.push_back(per);
	}

	for (person& p : people)
	{
		Color c;
		c.r = 255;
		c.g = 128;
		c.b = 128;
		p.display->color = c;
		p.display->setText(p.display->text);
	}

	helpDisplay->setText("Lobby: " + l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ") " + (isHost ? "You are the host!" : ""));
	//warningDisplay->setText("");
	Game::DiscordUpdatePresence(CurrentLobby.LobbyName, "Playing Multiplayer", "Average4K", MultiplayerLobby::CurrentLobby.Players, MultiplayerLobby::CurrentLobby.MaxPlayers, "");
	SteamFriends()->SetRichPresence("gamestatus", "Playing Multiplayer");
	SteamFriends()->SetRichPresence("steam_player_group", std::to_string(l.LobbyID).c_str());
	SteamFriends()->SetRichPresence("steam_player_group_size", std::to_string(l.PlayerList.size()).c_str());
	SteamFriends()->SetRichPresence("status", "In a lobby");
	SteamFriends()->SetRichPresence("connect", ("-joinLobby " + std::to_string(l.LobbyID)).c_str());
	STR_ENCRYPT_END
}

void MultiplayerLobby::onSteam(std::string s) {
	VM_START
	if (s == "chartAquired")
	{
		if (isAwaitingPack)
		{
			MainerMenu::selected = Game::steam->downloadedPack;
			MainerMenu::currentSelectedSong = MainerMenu::selected.songs[MainerMenu::packSongIndex].c.meta;
		}
		else
		{
			MainerMenu::currentSelectedSong = Game::steam->downloadedChart;
		}
		CPacketClientChartAcquired acquired;
		acquired.PacketType = eCPacketClientChartAcquired;
		acquired.Order = 0;
		downloading = false;
		if (isAwaitingPack)
			warningDisplay->setText("Pack downloaded! Current chart: " + MainerMenu::currentSelectedSong.meta.songName + " (" + MainerMenu::currentSelectedSong.meta.difficulties[diff].name + ")");
		else
			warningDisplay->setText("Chart downloaded! Current chart: " + MainerMenu::currentSelectedSong.meta.songName + " (" + MainerMenu::currentSelectedSong.meta.difficulties[diff].name + ")");
		Color c;
		c.r = 128;
		c.g = 128;
		c.b = 255;
		isAwaitingPack = false;
		warningDisplay->color = c;
		Multiplayer::sendMessage<CPacketClientChartAcquired>(acquired);
		std::string path = MainerMenu::currentSelectedSong.meta.folder + "/" + MainerMenu::currentSelectedSong.meta.audio;
		if (SoundManager::getChannelByName("prevSong") != NULL)
		{
			Channel* ch = SoundManager::getChannelByName("prevSong");
			ch->stop();
			ch->free();
			SoundManager::removeChannel("prevSong");
		}
		Channel* ch = SoundManager::createChannel(path.c_str(), "prevSong");
		ch->play();
		ch->setVolume(Game::save->GetDouble("Music Volume"));
	}
	VM_END
}

void MultiplayerLobby::mouseWheel(float wheel)
{
	if (wheel < 0)
		scroll+= 10;
	else if (wheel > 0)
		scroll-= 10;

	if (scroll < 0)
		scroll = 0;

	if (scroll > scrollAdd)
		scroll = scrollAdd;
}

void MultiplayerLobby::onPacket(PacketType pt, char* data, int32_t length)
{
	if (!this)
		return;
	//MUTATE_START
	STR_ENCRYPT_START
	SPacketUpdateLobbyData update;
	SPacketWtfAmInReply reply;
	SPacketUpdateLobbyChart cc;
	SPacketStatus f;
	SPacketFuckYou fuckyou;
	CPacketHostStartGame start;
	msgpack::unpacked result;

	msgpack::object obj;
	switch (pt)
	{
	case eSPacketStatus:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(f);
		switch (f.code)
		{
		case 803:
			// we host pog
			std::cout << "host me" << std::endl;
			isHost = true;
			refreshLobby(CurrentLobby);
			break;
		case 1337:
			Color c;
			c.r = 0;
			c.g = 255;
			c.b = 0;

			if (!isHost)
				return;

			warningDisplay->color = c;
			warningDisplay->setText("Everyone has the chart, you can now start. (Press enter to start, LSHIFT to reselect chart)");
			break;
		case 8876:
			Color cc;
			cc.r = 255;
			cc.g = 0;
			cc.b = 0;
			warningDisplay->color = cc;

			if (!isHost)
				warningDisplay->setText("Unable to start, some players do not have the chart! " + std::string((MainerMenu::currentSelectedSong.meta.difficulties.size() != 0 ? "You have it!" : "You do not have it!")));
			else
				warningDisplay->setText("Unable to start, some players do not have the chart! press enter to start when they do, press shift to reselect.");
			break;
		}

		if (f.code >= 9000)
		{
			std::cout << "got status for " << f.code << std::endl;
			int p = f.code - 9000;
			if (p < people.size())
			{
				std::cout << "player " << p << " is now green!" << std::endl;

				Color c;
				c.r = 128;
				c.g = 255;
				c.b = 128;

				person& pe = people[p];

				pe.display->color = c;
				pe.display->setText(pe.display->text);
			}
		}

		break;
	case eSPacketUpdateLobbyData:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(update);

		std::cout << "refresh lobby" << std::endl;

		refreshLobby(update.Lobby);
		break;
	case eSPacketWtfAmInReply:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(reply);

		std::cout << "refresh lobby" << std::endl;

		isHost = reply.isHost;

		refreshLobby(reply.Lobby);
		break;
	case eSPacketUpdateLobbyChart:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(cc);
		MainerMenu::packSongIndex = cc.chartIndex;
		MainerMenu::selectedDiffIndex = cc.diff;
		diff = cc.diff;
		Color c;
		c.r = 128;
		c.g = 128;
		c.b = 255;
		warningDisplay->color = c;
		downloading = true;
		if (cc.isPack)
		{
			Game::steam->LoadWorkshopChart((uint64_t)cc.packID);
			isAwaitingPack = true;
		}
		else
			Game::steam->LoadWorkshopChart((uint64_t)cc.chartID);

		if (isAwaitingPack)
			warningDisplay->setText("Obtaining pack...");
		else
			warningDisplay->setText("Obtaining chart...");

		for (person& p : people)
		{
			Color c;
			c.r = 255;
			c.g = 128;
			c.b = 128;
			p.display->color = c;
			p.display->setText(p.display->text);
		}

		// tell the server we aint got it lol (if we dont :))
		break;
	case eSPacketStartLobbyGame:
		std::cout << "start!" << std::endl;
		Game::instance->transitionToMenu(new Gameplay());


		break;
	case eSPacketFuckYou:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(fuckyou);

		if (fuckyou.demotion)
		{
			Game::asyncShowErrorWindow("Host switch", "You are no longer the host.", false);
			isHost = false;
			helpDisplay->setText("Lobby: " + CurrentLobby.LobbyName + " (" + std::to_string(CurrentLobby.Players) + "/" + std::to_string(CurrentLobby.MaxPlayers) + ") " + (isHost ? "You are the host!" : ""));
		}
		else if (fuckyou.lobbyKick)
		{
			Game::asyncShowErrorWindow("Kicked!", "You have been kicked from the lobby!", true);
			MainerMenu::isInLobby = false;

			Game::instance->transitionToMenu(new MultiplayerLobbies());

			people.clear();
			inLobby = false;
		}
		break;

	}

	STR_ENCRYPT_END
	
}

MultiplayerLobby::MultiplayerLobby(lobby l, bool hosted, bool backFromSelect = false)
{
	waitingForStart = backFromSelect && hosted;
	isHost = hosted;
	CurrentLobby = l;
}

void MultiplayerLobby::create() {
	
	MUTATE_START

	if (!waitingForStart)
		MainerMenu::currentSelectedSong = Chart();
	playerList = new AvgGroup(0, 0, 1280, 720);
	AvgSprite* sprite = new AvgSprite(0, 0, Noteskin::getMenuElement(Game::noteskin, "darkmodebg.png"));
	add(sprite);
	AvgRect* rect = new AvgRect(0, 0, 1280, 720);
	rect->alpha = 0.3;
	add(rect);

	helpDisplay = new Text(24, 100, "Lobby: " + CurrentLobby.LobbyName + " (" + std::to_string(CurrentLobby.Players) + "/" + std::to_string(CurrentLobby.MaxPlayers) + ")", 24, "NotoSans-Regular");
	helpDisplay->create();
	add(helpDisplay);

	warningDisplay = new Text(24, 125, "", 16, "NotoSans-Regular");
	Color c;
	c.r = 255;
	c.g = 0;
	c.b = 0;
	warningDisplay->color = c;
	warningDisplay->create();
	add(warningDisplay);



	CPacketWtfAmIn fuck;
	fuck.Order = 0;
	fuck.PacketType = eCPacketWtfAmIn;

	Multiplayer::sendMessage<CPacketWtfAmIn>(fuck);

	add(playerList);

	refreshLobby(CurrentLobby);

	if (waitingForStart)
	{
		CPacketHostChangeChart chart;
		chart.chartID = MainerMenu::selectedSong.steamId;
		chart.packID = MainerMenu::selected.steamId;
		chart.isPack = chart.packID != 0;
		chart.chartIndex = MainerMenu::packSongIndex;
		std::cout << "telling the server to start " << MainerMenu::selectedSong.steamId << std::endl;
		chart.diff = MainerMenu::selectedDiffIndex;
		chart.Order = 0;
		chart.PacketType = eCPacketHostChangeChart;
		Multiplayer::sendMessage<CPacketHostChangeChart>(chart);

		CPacketClientChartAcquired acquired;
		acquired.PacketType = eCPacketClientChartAcquired;
		acquired.Order = 1;

		Multiplayer::sendMessage<CPacketClientChartAcquired>(acquired);
	}
	created = true;

	chat = new ChatObject(0, 0);
	chat->create();
	add(chat);

	MUTATE_END
}

void MultiplayerLobby::keyDown(SDL_KeyboardEvent event)
{
	
	CPacketHostStartGame start;
	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE:
			if (chat->typing || chat->opened)
				return;
			CPacketLeave leave;
			leave.Order = 0;
			leave.PacketType = eCPacketLeave;

			Multiplayer::sendMessage<CPacketLeave>(leave);

			MainerMenu::isInLobby = false;
			SteamFriends()->SetRichPresence("steam_player_group", nullptr);
			SteamFriends()->SetRichPresence("steam_player_group_size", nullptr);
			Game::instance->transitionToMenu(new MultiplayerLobbies());

			people.clear();
			inLobby = false;
			break;
		case SDLK_TAB:
			if (chat->typing)
				return;
			if (!chat->opened)
				chat->open();
			else
				chat->close();
			break;
		case SDLK_RETURN:
			if (chat->typing || chat->opened)
				return;
			if (!isHost)
				return;

			if (waitingForStart && MainerMenu::currentSelectedSong.meta.difficulties.size() != 0)
			{
				start.Order = 0;
				start.PacketType = eCPacketHostStartGame;

				Multiplayer::sendMessage<CPacketHostStartGame>(start);
			}
			else
			{
				MainerMenu::isInLobby = true;
				Game::instance->transitionToMenu(new MainerMenu());

				people.clear();

			}
			break;
		case SDLK_LSHIFT:
			if (chat->typing || chat->opened)
				return;
			if (!isHost && !waitingForStart)
				return;
			MainerMenu::isInLobby = true;
			Game::instance->transitionToMenu(new MainerMenu());


			people.clear();

			break;
	}
	
}

void MultiplayerLobby::update(Events::updateEvent event)
{
	MUTATE_START
	int i = 0;

	warningDisplay->y = 125 - scroll;
	helpDisplay->y = 100 - scroll;

	for (person& p : people)
	{
		AvgSprite* spr = p.avatar;
		spr->x = p.display->x - 58;
		p.display->y = (192 + (46 * i)) - scroll;
		spr->y = (p.display->y - 8);
		spr->w = 46;
		spr->h = 46;
		i++;
	}

	if (downloading)
	{
		float prog = Steam::CheckWorkshopDownload();
		if (prog != 0)
		{
			if (isAwaitingPack)
				warningDisplay->setText("Obtaining Pack... (" + std::to_string(prog * 100).substr(0, 3) + ")");
			else
				warningDisplay->setText("Obtaining chart... (" + std::to_string(prog * 100).substr(0, 3) + ")");
			Color c;
			c.r = 128;
			c.g = 128;
			c.b = 255;
			warningDisplay->color = c;
		}
	}
	MUTATE_END
}