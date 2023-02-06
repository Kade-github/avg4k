#pragma once
#include "Packet.h"
#include "Includes.h"
class CPacketLeaderboardRequest : public Packet
{
public:
	unsigned long chartId;
	int diffIndex;

	long chartIndex;

	MSGPACK_DEFINE_MAP(chartId, diffIndex, chartIndex);
};