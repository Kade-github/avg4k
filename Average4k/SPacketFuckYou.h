#pragma once
#include "Packet.h"
#include "includes.h"
#include "Lobby.h"
class SPacketFuckYou : public Packet
{
public:
	bool demotion;
	bool lobbyKick;
	lobby lobby;
	MSGPACK_DEFINE_MAP(demotion, lobbyKick, lobby);
};