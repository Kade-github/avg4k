#pragma once

#ifndef SPacketFuckYou_H
#define SPacketFuckYou_H

#include "Packet.h"
#include "Lobby.h"

namespace Average4k::Multiplayer::Packets
{

	class SPacketFuckYou : public Packet
	{
	public:
		bool demotion;
		bool lobbyKick;
		lobby lobby;
		MSGPACK_DEFINE_MAP(demotion, lobbyKick, lobby);
	};
}

#endif