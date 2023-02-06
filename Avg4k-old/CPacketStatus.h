#pragma once
#include "Packet.h"
class CPacketStatus : public Packet
{
public:
	unsigned int code;

	std::string Status;

	MSGPACK_DEFINE_MAP(code, Status);
};