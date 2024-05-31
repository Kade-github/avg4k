#pragma once

#ifndef CPACKETSTATUS_H
#define CPACKETSTATUS_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{

	class CPacketStatus : public Packet
	{
	public:
		unsigned int code;

		std::string Status;

		MSGPACK_DEFINE_MAP(code, Status);
	};
}
#endif