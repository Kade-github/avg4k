#pragma once
#include "Packet.h"
#include "Lobby.h"
class SPacketWtfAmInReply : public Packet
{
public:
	lobby Lobby;

	MSGPACK_DEFINE_MAP(Lobby);
};