#pragma once
#include "Packet.h"
#include "includes.h"
class CPacketHostChangeChart : public Packet
{
public:
	long long chartID;
	int diff;
	MSGPACK_DEFINE_MAP(chartID, diff);
};