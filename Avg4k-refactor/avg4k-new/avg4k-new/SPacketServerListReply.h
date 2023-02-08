#pragma once
#include "Packet.h"
#include "Lobby.h"
class SPacketServerListReply : public Packet
{
public:
	std::vector<lobby> Lobbies;

	MSGPACK_DEFINE_MAP(Lobbies);
};