#pragma once

#ifndef LOBBY_H
#define LOBBY_H
#include <msgpack.hpp>

namespace Average4k::Multiplayer
{
	struct paramPlayer {
		std::string* Name;
		std::string SteamID64;
		std::string* AvatarURL;
	};

	class player {
	public:
		std::string Name;
		std::string SteamID64;
		std::string AvatarURL;
		std::string Avatar;
		MSGPACK_DEFINE_MAP(Name, SteamID64, AvatarURL, Avatar);
	};


	class lobby {
	public:
		unsigned long LobbyID;
		std::string LobbyName;
		player Host;
		int MaxPlayers;
		int Players;
		bool IsStarted;
		std::vector<player> PlayerList;

		MSGPACK_DEFINE_MAP(LobbyID, LobbyName, Host, MaxPlayers, Players, IsStarted, PlayerList);
	};
}

#endif