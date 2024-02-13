#pragma once
#ifndef CPACKETHELLO_H
#define CPACKETHELLO_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{
	class CPacketHello : public Packet
	{
	public:
		std::string SteamTicket;

		MSGPACK_DEFINE_MAP(SteamTicket);
	};
}

#endif