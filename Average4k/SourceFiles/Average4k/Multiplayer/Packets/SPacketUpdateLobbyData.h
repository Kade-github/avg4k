#pragma once

#ifndef SPacketUpdateLobbyData_H
#define SPacketUpdateLobbyData_H

#include "Packet.h"
#include "Lobby.h"

namespace Average4k::Multiplayer::Packets
{

	class SPacketUpdateLobbyData : public Packet
	{
	public:
		lobby Lobby;

		MSGPACK_DEFINE_MAP(Lobby);
	};
}

#endif