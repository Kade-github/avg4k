#include "MultiplayerLobby.h"
#include "SPacketUpdateLobbyData.h"

struct PARAMETERS
{
	paramPlayer* pl;
	person* p;
};

DWORD CALLBACK getSteamAvatar(LPVOID param)
{
	PARAMETERS* params = (PARAMETERS*)param;

	SDL_Texture* t = Steam::getAvatar(params->pl->AvatarURL->c_str());
	if (params->p->display)
		params->p->avatar = t;
	return 0;
}

void MultiplayerLobby::refreshLobby(lobby l)
{
	CurrentLobby = l;

	for (person* p : people)
	{
		p->display->die();
		SDL_DestroyTexture(p->avatar);
		delete p;
	}

	people.clear();

	for (int i = 0; i < l.PlayerList.size(); i++)
	{
		player& p = l.PlayerList[i];
		person* per = (person*)malloc(sizeof(person));
		per->avatar = SDL_CreateTexture(Game::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 46,46);
		PARAMETERS* params = (PARAMETERS*)malloc(sizeof(PARAMETERS));

		paramPlayer* pp = (paramPlayer*)malloc(sizeof(player));
		pp->AvatarURL = new std::string(p.AvatarURL);
		pp->Name = new std::string(p.Name);
		pp->SteamID64 = p.SteamID64;
		params->pl = pp;

		params->p = per;
		CreateThread(NULL, 0, getSteamAvatar, &params, 0, NULL);
		per->display = new Text(82,192 + (46 * i), p.Name, 10, 10);
		per->display->create();
		people.push_back(per);
	}
	
}

void MultiplayerLobby::onPacket(PacketType pt, char* data, int32_t length)
{
	SPacketUpdateLobbyData update;
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
	}
}

MultiplayerLobby::MultiplayerLobby(lobby l)
{
	helpDisplay = new Text(24, 100, "Lobby: " + l.LobbyName + " (" + std::to_string(l.Players) + "/" + std::to_string(l.MaxPlayers) + ")", 10, 10);
	helpDisplay->create();

	refreshLobby(l);
}

void MultiplayerLobby::keyDown(SDL_KeyboardEvent event)
{
}

void MultiplayerLobby::update(Events::updateEvent event)
{
	for (person* p : people)
	{
		SDL_FRect avat;
		avat.x = p->display->x - 58;
		avat.y = p->display->y;
		avat.w = 46;
		avat.h = 46;

		SDL_RenderCopyF(Game::renderer, p->avatar, NULL, &avat);

		SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);

		SDL_RenderDrawRectF(Game::renderer, &avat);

		SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 255);
	}
}
