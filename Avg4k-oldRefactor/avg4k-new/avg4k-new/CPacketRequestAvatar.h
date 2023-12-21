#pragma once
#include "Packet.h"
#include "includes.h"
class CPacketRequestAvatar : public Packet
{
public:
	std::string steamid64;

	MSGPACK_DEFINE_MAP(steamid64);
};