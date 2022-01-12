#pragma once
#include "Packet.h"
class SPacketHello : public Packet
{
public:
	std::string Message;

	bool Error;

	int ErrorCode;

	std::string reauth;
	std::string avatarURL;

	MSGPACK_DEFINE_MAP(Message, Error, ErrorCode, reauth, avatarURL);
};