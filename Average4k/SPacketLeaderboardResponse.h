#pragma once
#include "Packet.h"
#include "includes.h"

class LeaderboardEntry {
public:
	float accuracy;

	std::string username;

	std::string steamid;

	MSGPACK_DEFINE_MAP(accuracy, username, steamid);
};

class Leaderboard {
public:
	long chartIndex;

	unsigned long ChartId;

	std::vector<LeaderboardEntry> entries;

	MSGPACK_DEFINE_MAP(chartIndex, ChartId, entries);
};

class SPacketLeaderboardResponse : public Packet
{
public:
	Leaderboard leaderboard;

	MSGPACK_DEFINE_MAP(leaderboard);
};