#pragma once

#ifndef SPacketStatus_H
#define SPacketStatus_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{
	class SPacketStatus : public Packet
	{
	public:
		unsigned int code;

		std::string Status;

		MSGPACK_DEFINE_MAP(code, Status);
	};
}

#endif