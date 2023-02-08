#pragma once
#include "Packet.h"
class SPacketHostServerReply : public Packet
{
	public:
		unsigned long lobbyId;

		MSGPACK_DEFINE_MAP(lobbyId);
};