#pragma once
#include "Packet.h"
#include "includes.h"
class CPacketHostChangeChart : public Packet
{
public:
	bool isPack;
	uint64_t packID;
	uint64_t chartID;
	int chartIndex;
	int diff;
	MSGPACK_DEFINE_MAP(isPack, packID, chartID, chartIndex, diff);
};