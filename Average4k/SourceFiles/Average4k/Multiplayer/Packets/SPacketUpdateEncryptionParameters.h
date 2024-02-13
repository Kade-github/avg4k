#pragma once

#ifndef SPacketUpdateEncryptionParameters_H
#define SPacketUpdateEncryptionParameters_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{
	class SPacketUpdateEncryptionParameters : public Packet
	{
	public:
		std::string key;

		MSGPACK_DEFINE_MAP(key);
	};
}

#endif