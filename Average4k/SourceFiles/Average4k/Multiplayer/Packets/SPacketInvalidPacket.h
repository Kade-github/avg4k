#pragma once

#ifndef SPacketInvalidPacket_H
#define SPacketInvalidPacket_H

#include "Packet.h"
namespace Average4k::Multiplayer::Packets
{
	class SPacketInvalidPacket : public Packet
	{
	public:
		unsigned int orderID;

		MSGPACK_DEFINE_MAP(orderID);
	};
}

#endif