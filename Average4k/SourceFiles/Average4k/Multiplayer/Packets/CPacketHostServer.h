#pragma once

#ifndef CPACKETHOSTSERVER_H
#define CPACKETHOSTSERVER_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{

	class CPacketHostServer : public Packet
	{
	public:
		std::string LobbyName;

		MSGPACK_DEFINE_MAP(LobbyName);
	};
}

#endif