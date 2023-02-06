#pragma once
#include "Packet.h"
class SPacketInvalidPacket : public Packet
{
public:
	unsigned int orderID;

	MSGPACK_DEFINE_MAP(orderID);
};