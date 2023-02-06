#pragma once
#include "Packet.h"
#include "Lobby.h"
class CPacketHostUpdateLobby :
    public Packet
{
public:
    lobby Lobby;
    MSGPACK_DEFINE_MAP(Lobby);
};

