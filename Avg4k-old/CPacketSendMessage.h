#pragma once
#include "Packet.h"
#include "includes.h"

class CPacketSendMessage : public Packet {
public:
	std::string message;

	MSGPACK_DEFINE_MAP(message);
};