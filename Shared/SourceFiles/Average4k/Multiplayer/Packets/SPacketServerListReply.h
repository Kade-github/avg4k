#pragma once

#ifndef SPACKETSERVERLISTREPLY_H
#define SPACKETSERVERLISTREPLY_H

#include "Packet.h"
#include "Lobby.h"

namespace Average4k::Multiplayer::Packets
{

	class SPacketServerListReply : public Packet
	{
	public:
		std::vector<lobby> Lobbies;

		MSGPACK_DEFINE_MAP(Lobbies);
	};
}
#endif