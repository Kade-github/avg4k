#include "MultiplayerLobby.h"
#include "AvgSprite.h"

bool MultiplayerLobby::inLobby = false;
lobby MultiplayerLobby::CurrentLobby;
bool MultiplayerLobby::isHost = false;


void MultiplayerLobby::refreshLobby(lobby l)
{
	CurrentLobby = l;
	inLobby = true;

	for (person p : people)
	{
		p.display->destroy();
		SDL_DestroyTexture(p.avatar);
	}

	people.clear();

	for (int i = 0; i < l.PlayerList.size(); i++)
	{
		player& p = l.PlayerList[i];
		person per;
		per.display = new Text(82, 192 + (46 * i), p.Name, 24);
		per.display->create();
		SDL_Texture* t = Steam::getAvatar(p.AvatarURL.c_str());
		if (t)
			per.avatar = t;
		else
			per.avatar = NULL;
		people.push_back(per);
	}

	helpDisplay->setText("Lobby: " + l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ") " + (isHost ? "You are the host!" : ""));
	
}

void MultiplayerLobby::onSteam(std::string s) {
	if (s == "chartAquired")
	{
		CPacketClientChartAcquired acquired;
		acquired.PacketType = eCPacketClientChartAcquired;
		acquired.Order = 0;

		Multiplayer::sendMessage<CPacketClientChartAcquired>(acquired);
	}
}

void MultiplayerLobby::onPacket(PacketType pt, char* data, int32_t length)
{
	if (!this)
		return;
	SPacketUpdateLobbyData update;
	SPacketWtfAmInReply reply;
	SPacketUpdateLobbyChart cc;
	SPacketStatus f;
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
		case 8876:
			warningDisplay->setText("Unable to start, some players do not have the chart! press enter to start when they do, press shift to reselect.");
			break;
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

		if (waitingForStart)
		{
			start.Order = 0;
			start.PacketType = eCPacketHostStartGame;

			Multiplayer::sendMessage<CPacketHostStartGame>(start);
		}
		break;
	case eSPacketUpdateLobbyChart:
		msgpack::unpack(result, data, length);

		obj = msgpack::object(result.get());

		obj.convert(cc);

		SongSelect::selectedDiffIndex = cc.diff;

		Game::steam->LoadWorkshopChart((uint64_t)cc.chartID);

		// tell the server we aint got it lol (if we dont :))
		break;
	case eSPacketStartLobbyGame:
		std::cout << "start!" << std::endl;
		
		Game::currentMenu = new Gameplay();
		SongSelect::currentChart = Game::steam->downloadedChart;
		helpDisplay->destroy();
		
		for (person p : people)
		{
			p.display->destroy();
			SDL_DestroyTexture(p.avatar);
		}

		people.clear();
		removeAll();
		break;
	}
}

MultiplayerLobby::MultiplayerLobby(lobby l, bool hosted, bool backFromSelect = false)
{
	AvgSprite* sprite = new AvgSprite(0, 0, "assets/graphical/menu/bg.png");
	sprite->create();
	add(sprite);
	isHost = hosted;
	helpDisplay = new Text(24, 100, "Lobby: " + l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ")", 24);
	helpDisplay->create();

	warningDisplay = new Text(24, 125, "", 16);
	Color c;
	c.r = 255;
	c.g = 0;
	c.b = 0;
	warningDisplay->color = c;
	warningDisplay->create();

	CPacketWtfAmIn fuck;
	fuck.Order = 0;
	fuck.PacketType = eCPacketWtfAmIn;

	Multiplayer::sendMessage<CPacketWtfAmIn>(fuck);

	waitingForStart = backFromSelect && hosted;

	if (waitingForStart)
	{
		CPacketHostChangeChart chart;
		chart.chartID = (uint64_t) SongSelect::selectedSong->steamHandle;
		std::cout << "telling the server to start " << SongSelect::selectedSong->steamHandle << std::endl;
		chart.diff = SongSelect::selectedDiffIndex;
		chart.Order = 0;
		chart.PacketType = eCPacketHostChangeChart;
		Multiplayer::sendMessage<CPacketHostChangeChart>(chart);

		CPacketClientChartAcquired acquired;
		acquired.PacketType = eCPacketClientChartAcquired;
		acquired.Order = 1;

		Multiplayer::sendMessage<CPacketClientChartAcquired>(acquired);
	}
}

void MultiplayerLobby::keyDown(SDL_KeyboardEvent event)
{
	CPacketHostStartGame start;
	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE:
			CPacketLeave leave;
			leave.Order = 0;
			leave.PacketType = eCPacketLeave;

			Multiplayer::sendMessage<CPacketLeave>(leave);

			helpDisplay->destroy();

			Game::currentMenu = new MultiplayerLobbies();
			for (person p : people)
			{
				p.display->destroy();
				SDL_DestroyTexture(p.avatar);
			}
			warningDisplay->destroy();
			people.clear();
			removeAll();
			inLobby = false;
			break;
		case SDLK_RETURN:
			if (!isHost)
				return;

			if (waitingForStart)
			{
				start.Order = 0;
				start.PacketType = eCPacketHostStartGame;

				Multiplayer::sendMessage<CPacketHostStartGame>(start);
			}
			else
			{

				Game::currentMenu = new SongSelect();
				helpDisplay->destroy();

				for (person p : people)
				{
					p.display->destroy();
					SDL_DestroyTexture(p.avatar);
				}

				warningDisplay->destroy();

				people.clear();
				removeAll();

			}
			break;
		case SDLK_LSHIFT:
			if (!isHost && !waitingForStart)
				return;
			Game::currentMenu = new SongSelect();
			helpDisplay->destroy();

			for (person p : people)
			{
				p.display->destroy();
				SDL_DestroyTexture(p.avatar);
			}

			warningDisplay->destroy();

			people.clear();
			removeAll();

			break;
	}
}

void MultiplayerLobby::update(Events::updateEvent event)
{
	
}

void MultiplayerLobby::postUpdate(Events::updateEvent event)
{
	for (person p : people)
	{
		SDL_FRect avat;
		avat.x = p.display->x - 58;
		avat.y = p.display->y - 8;
		avat.w = 46;
		avat.h = 46;

		SDL_SetTextureAlphaMod(p.avatar, 255);

		SDL_RenderCopyF(Game::renderer, p.avatar, NULL, &avat);

		SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);

		SDL_RenderDrawRectF(Game::renderer, &avat);

		SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
	}
}