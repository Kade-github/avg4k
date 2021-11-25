#include "MultiplayerLobby.h"
#include "SPacketUpdateLobbyData.h"
#include "SPacketWtfAmInReply.h"
#include "CPacketWtfAmIn.h"


void MultiplayerLobby::refreshLobby(lobby l)
{
	CurrentLobby = l;

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
	
}

void MultiplayerLobby::onPacket(PacketType pt, char* data, int32_t length)
{
	SPacketUpdateLobbyData update;
	SPacketWtfAmInReply reply;
	SPacketStatus f;
	msgpack::unpacked result;

	msgpack::object obj;
	switch (pt)
	{
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
	}
}

MultiplayerLobby::MultiplayerLobby(lobby l)
{
	helpDisplay = new Text(24, 100, "Lobby: " + l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ")", 10, 10);
	helpDisplay->create();

	CPacketWtfAmIn fuck;
	fuck.Order = 0;
	fuck.PacketType = eCPacketWtfAmIn;

	Multiplayer::sendMessage<CPacketWtfAmIn>(fuck);
}

void MultiplayerLobby::keyDown(SDL_KeyboardEvent event)
{
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
