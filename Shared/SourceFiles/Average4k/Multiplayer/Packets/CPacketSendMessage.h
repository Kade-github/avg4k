#pragma once

#ifndef CPACKETSENDMESSAGE_H
#define CPACKETSENDMESSAGE_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{
	class CPacketSendMessage : public Packet {
	public:
		std::string message;

		MSGPACK_DEFINE_MAP(message);
	};
};

#endif