#pragma once

#ifndef CPACKETHOSTUPDATELOBBY_H
#define CPACKETHOSTUPDATELOBBY_H

#include "Packet.h"
#include "Lobby.h"

namespace Average4k::Multiplayer::Packets
{

    class CPacketHostUpdateLobby :
        public Packet
    {
    public:
        lobby Lobby;
        MSGPACK_DEFINE_MAP(Lobby);
    };

}

#endif