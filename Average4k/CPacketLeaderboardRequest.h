#pragma once
#include "Packet.h"
#include "Includes.h"
class CPacketLeaderboardRequest : public Packet
{
public:
	unsigned long chartId;

	long chartIndex;

	MSGPACK_DEFINE_MAP(chartId, chartIndex);
};