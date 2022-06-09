#pragma once
#include "Packet.h"
#include "includes.h"

struct LeaderboardEntry {
	float accuracy;

	std::string username;

	std::string steamid;

	MSGPACK_DEFINE_MAP(accuracy, username, steamid);
};

struct Leaderboard {
	long chartIndex;

	unsigned long ChartId;

	std::vector<LeaderboardEntry> entires;

	MSGPACK_DEFINE_MAP(chartIndex, ChartId, entires);
};

class SPacketLeaderboardResponse : public Packet
{
public:
	Leaderboard leaderboard;

	MSGPACK_DEFINE_MAP(leaderboard);
};