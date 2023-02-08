#pragma once
#include "Packet.h"
class CPacketJoinServer : public Packet
{
	public:
		unsigned long LobbyID;

		MSGPACK_DEFINE_MAP(LobbyID);
};