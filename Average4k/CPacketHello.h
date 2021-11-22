#pragma once
#include "Packet.h"
class CPacketHello : public Packet
{
public:
	std::string SteamTicket;

	MSGPACK_DEFINE_MAP(SteamTicket);
};