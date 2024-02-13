#pragma once

#ifndef CPACKETREQUESTAVATAR_H
#define CPACKETREQUESTAVATAR_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{

	class CPacketRequestAvatar : public Packet
	{
	public:
		std::string steamid64;

		MSGPACK_DEFINE_MAP(steamid64);
	};
};

#endif