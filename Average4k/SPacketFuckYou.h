#pragma once
#include "Packet.h"
#include "includes.h"
class SPacketFuckYou : public Packet
{
public:
	bool demotion;
	bool lobbyKick;
	MSGPACK_DEFINE_MAP(demotion, lobbyKick);
};