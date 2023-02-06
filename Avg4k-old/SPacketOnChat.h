#pragma once
#include "Packet.h"
#include "includes.h"
#include "Lobby.h"

class ccolor {
public:
	float red;
	float green;
	float blue;
	MSGPACK_DEFINE_MAP(red, green, blue);
};

class SPacketOnChat : public Packet {
public:
	ccolor color;
	ccolor tagColor;
	std::string tagText;
	std::string message;
	player sender;

	MSGPACK_DEFINE_MAP(message, color, sender, tagColor, tagText);
};