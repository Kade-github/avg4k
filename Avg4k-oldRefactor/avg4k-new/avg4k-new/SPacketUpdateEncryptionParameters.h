#pragma once
#include "Packet.h"
#include <string>
class SPacketUpdateEncryptionParameters : public Packet
{
public:
	std::string key;

	MSGPACK_DEFINE_MAP(key);
};