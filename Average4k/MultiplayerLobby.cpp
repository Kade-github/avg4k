#include "MultiplayerLobby.h"
#include "CPacketHostStartGame.h"

bool MultiplayerLobby::inLobby = false;
lobby MultiplayerLobby::CurrentLobby;
bool MultiplayerLobby::isHost = false;


void MultiplayerLobby::refreshLobby(lobby l)
{
	CurrentLobby = l;
	inLobby = true;

	for (person p : people)
	{
		p.display->die();
		SDL_DestroyTexture(p.avatar);
	}

	people.clear();

	for (int i = 0; i < l.PlayerList.size(); i++)
	{
		player& p = l.PlayerList[i];
		person per;
		per.display = new Text(82, 192 + (46 * i), p.Name, 10, 10);
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

void MultiplayerLobby::onPacket(PacketType pt, char* data, int32_t length)
{
	if (!this)
		return;
	SPacketUpdateLobbyData update;
	SPacketWtfAmInReply reply;
	SPacketUpdateLobbyChart cc;
	SPacketStatus f;
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
			isHost = true;
			refreshLobby(CurrentLobby);
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

		refreshLobby(reply.Lobby);
		break;
	case eSPacketUpdateLobbyChart:
		// tell the server we aint got it lol (if we dont :))
		break;
	case eSPacketStartLobbyGame:
		std::cout << "start!" << std::endl;
		QuaverFile* file = new QuaverFile();
		chartMeta meta = file->returnChart("assets/charts/Rozebud - Philly Nice [B-Side Remix] - 53269");
		MainMenu::currentChart = new Chart(meta);
		Game::currentMenu = new Gameplay();
		helpDisplay->die();

		for (person p : people)
		{
			p.display->die();
			SDL_DestroyTexture(p.avatar);
		}

		people.clear();
		break;
	}
}

MultiplayerLobby::MultiplayerLobby(lobby l, bool hosted)
{
	isHost = hosted;
	helpDisplay = new Text(24, 100, "Lobby: " + l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ")", 10, 10);
	helpDisplay->create();

	CPacketWtfAmIn fuck;
	fuck.Order = 0;
	fuck.PacketType = eCPacketWtfAmIn;

	Multiplayer::sendMessage<CPacketWtfAmIn>(fuck);
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

			helpDisplay->die();

			Game::currentMenu = new MultiplayerLobbies();
			for (person p : people)
			{
				p.display->die();
				SDL_DestroyTexture(p.avatar);
			}

			people.clear();
			inLobby = false;
			break;
		case SDLK_RETURN:
			if (!isHost)
				return;

			std::cout << "hello" << std::endl;

			start.Order = 0;
			start.PacketType = eCPacketHostStartGame;

			Multiplayer::sendMessage<CPacketHostStartGame>(start);
			break;
	}
}

void MultiplayerLobby::update(Events::updateEvent event)
{
	for (person p : people)
	{
		SDL_FRect avat;
		avat.x = p.display->x - 58;
		avat.y = p.display->y - 8;
		avat.w = 46;
		avat.h = 46;

		SDL_RenderCopyF(Game::renderer, p.avatar, NULL, &avat);

		SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);

		SDL_RenderDrawRectF(Game::renderer, &avat);

		SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
	}
}
