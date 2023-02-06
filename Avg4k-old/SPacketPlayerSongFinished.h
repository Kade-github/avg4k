#pragma once
#include "Packet.h"
#include "Lobby.h"
class SPacketPlayerSongFinished :
    public Packet
{
public:
    player Player;

    MSGPACK_DEFINE_MAP(Player);
};

