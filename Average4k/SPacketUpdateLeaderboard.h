#pragma once
#include "Packet.h"
#include "includes.h"

class PlayerScore {
public:
	long score;
	float meanTiming;
	std::string Username;
	std::string SteamAvatar;
	unsigned long SteamID64;
	unsigned short Ranking;
	MSGPACK_DEFINE_MAP(score, meanTiming, Username, SteamAvatar, SteamID64, Ranking);
};

class SPacketUpdateLeaderboard : public Packet
{
public:
	std::vector<PlayerScore> orderedScores;

	MSGPACK_DEFINE_MAP(orderedScores);
};