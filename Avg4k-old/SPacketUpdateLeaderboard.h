#pragma once
#include "Packet.h"
#include "includes.h"

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

class SPacketUpdateLeaderboard : public Packet
{
public:
	std::vector<PlayerScore> orderedScores;

	MSGPACK_DEFINE_MAP(orderedScores);
};