#pragma once
#include "Packet.h"
class SPacketHello : public Packet
{
public:
	std::string Message;

	bool Error;

	int ErrorCode;

	std::string reauth;

	MSGPACK_DEFINE_MAP(Message, Error, ErrorCode, reauth);
};