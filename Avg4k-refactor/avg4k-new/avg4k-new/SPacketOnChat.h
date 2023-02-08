#pragma once
#include "Packet.h"
#include "includes.h"

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
	// TODO: add player

	MSGPACK_DEFINE_MAP(message, color, tagColor, tagText);
};