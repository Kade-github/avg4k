#pragma once

#ifndef SPacketHostServerReply_H
#define SPacketHostServerReply_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{
	class SPacketHostServerReply : public Packet
	{
	public:
		unsigned long lobbyId;

		MSGPACK_DEFINE_MAP(lobbyId);
	};
}
#endif