#pragma once

#ifndef SPACKETPLAYERSONGFINISHED_H
#define SPACKETPLAYERSONGFINISHED_H

#include "Packet.h"
#include "Lobby.h"
namespace Average4k::Multiplayer::Packets
{
    class SPacketPlayerSongFinished :
        public Packet
    {
    public:
        player Player;

        MSGPACK_DEFINE_MAP(Player);
    };
}

#endif