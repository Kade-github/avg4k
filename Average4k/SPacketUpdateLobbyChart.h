#pragma once
#include "Packet.h"
#include "includes.h"
class SPacketUpdateLobbyChart : public Packet
{
public:
	std::string chartID;
	int diff;
	MSGPACK_DEFINE_MAP(chartID, diff);
};