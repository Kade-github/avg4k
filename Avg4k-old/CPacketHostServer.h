#pragma once
#include "Packet.h"
class CPacketHostServer : public Packet
{
	public:
		 std::string LobbyName;

		 MSGPACK_DEFINE_MAP(LobbyName);
};