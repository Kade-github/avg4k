#pragma once
#include "Packet.h"
#include "includes.h"
class CPacketHostChangeChart : public Packet
{
public:
	uint64_t chartID;
	int diff;
	MSGPACK_DEFINE_MAP(chartID, diff);
};