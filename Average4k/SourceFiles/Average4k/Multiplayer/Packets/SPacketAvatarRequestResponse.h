#pragma once

#ifndef SPacketAvatarRequestResponse_H
#define SPacketAvatarRequestResponse_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{

	class SPacketAvatarRequestResponse : public Packet
	{
	public:
		std::string url;
		std::string SteamID64;

		MSGPACK_DEFINE_MAP(url, SteamID64);
	};
}