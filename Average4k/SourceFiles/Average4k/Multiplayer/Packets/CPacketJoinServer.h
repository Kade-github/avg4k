#pragma once

#ifndef CPACKETJOINSERVER_H
#define CPACKETJOINSERVER_H
#include "Packet.h"
namespace Average4k::Multiplayer::Packets
{
	class CPacketJoinServer : public Packet
	{
	public:
		unsigned long LobbyID;

		MSGPACK_DEFINE_MAP(LobbyID);
	};
}

#endif