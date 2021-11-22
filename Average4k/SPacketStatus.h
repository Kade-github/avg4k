#pragma once
#include "Packet.h"
class SPacketStatus : public Packet
{
public:
	unsigned int code;

	std::string Status;

	MSGPACK_DEFINE_MAP(code, Status);
};