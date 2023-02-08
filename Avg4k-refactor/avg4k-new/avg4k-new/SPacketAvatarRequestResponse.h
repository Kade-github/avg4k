#pragma once
#include "Packet.h"
#include "includes.h"
class SPacketAvatarRequestResponse : public Packet
{
public:
	std::string url;
	std::string SteamID64;

	MSGPACK_DEFINE_MAP(url, SteamID64);
};