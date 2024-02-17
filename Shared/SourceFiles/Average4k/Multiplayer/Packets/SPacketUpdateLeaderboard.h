#pragma once

#ifndef SPACKETUPDATELEADERBOARD_H
#define SPACKETUPDATELEADERBOARD_H

#include "Packet.h"

namespace Average4k::Multiplayer
{
	class PlayerScore {
	public:
		long score;
		float meanTiming;
		std::string Username;
		std::string SteamAvatar;
		std::string SteamID64;
		float Accuracy;
		unsigned short Ranking;
		MSGPACK_DEFINE_MAP(score, meanTiming, Username, SteamAvatar, SteamID64, Ranking, Accuracy);
	};
}

namespace Average4k::Multiplayer::Packets
{
	class SPacketUpdateLeaderboard : public Packet
	{
	public:
		std::vector<PlayerScore> orderedScores;

		MSGPACK_DEFINE_MAP(orderedScores);
	};
}

#endif