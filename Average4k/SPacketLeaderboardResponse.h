#pragma once
#include "Packet.h"
#include "includes.h"

class LeaderboardEntry {
public:
	std::map<float, float> noteTiming;

	int combo;

	float accuracy;

	std::string username;

	std::string steamid;

	bool online;

	MSGPACK_DEFINE_MAP(noteTiming, accuracy, username, steamid);
};

class Leaderboard {
public:
	long chartIndex;
	long diffIndex;

	unsigned long ChartId;

	std::vector<LeaderboardEntry> entries;

	MSGPACK_DEFINE_MAP(chartIndex, diffIndex, ChartId, entries);
};

class SPacketLeaderboardResponse : public Packet
{
public:
	Leaderboard leaderboard;

	MSGPACK_DEFINE_MAP(leaderboard);
};