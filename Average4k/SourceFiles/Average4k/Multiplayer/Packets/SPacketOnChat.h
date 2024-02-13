#pragma once

#ifndef SPacketOnChat_h
#define SPacketOnChat_h

#include "Packet.h"

namespace Average4k::Multiplayer
{
	class ccolor {
	public:
		float red;
		float green;
		float blue;
		MSGPACK_DEFINE_MAP(red, green, blue);
	};
}

namespace Average4k::Multiplayer::Packets
{

	class SPacketOnChat : public Packet {
	public:
		ccolor color;
		ccolor tagColor;
		std::string tagText;
		std::string message;
		// TODO: add player

		MSGPACK_DEFINE_MAP(message, color, tagColor, tagText);
	};
}

#endif