#pragma once
#include "includes.h"
#include <msgpack.hpp>

class player {
	public:
		std::string Name;
		unsigned long SteamID64;
		std::string AvatarURL;
		MSGPACK_DEFINE_MAP(Name, SteamID64, AvatarURL);
};


class lobby {
	public:
		unsigned long LobbyID;
		std::string LobbyName;
		int MaxPlayers;
		int Players;
		std::vector<player> PlayerList;

		MSGPACK_DEFINE_MAP(LobbyID, LobbyName, MaxPlayers, Players, PlayerList);
};