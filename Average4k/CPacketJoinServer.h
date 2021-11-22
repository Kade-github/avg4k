#pragma once
#include "Packet.h"
class CPacketJoinServer : public Packet
{
	public:
		unsigned int lobbyId;

		MSGPACK_DEFINE_MAP(lobbyId);
};