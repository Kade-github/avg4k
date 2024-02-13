#pragma once

#ifndef SPACKETWTFAMINREPLY_H
#define SPACKETWTFAMINREPLY_H

#include "Packet.h"
#include "Lobby.h"

namespace Average4k::Multiplayer::Packets
{

	class SPacketWtfAmInReply : public Packet
	{
	public:
		lobby Lobby;
		bool isHost;

		MSGPACK_DEFINE_MAP(Lobby, isHost);
	};
}

#endif