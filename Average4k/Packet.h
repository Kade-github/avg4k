#pragma once


enum PacketType
{
    //Client Packets
    eCPacketHello = 1,
    eCPacketServerList = 2,
    eCPacketJoinServer = 3,
    eCPacketHostServer = 4,
    eCPacketHostChangeChart = 5,
    eCPacketClientMissingChart = 6,
    eCPacketHostStartGame = 7,
    eCPacketNoteHit = 8,
    eCPacketSongFinished = 9,
    eCPacketStatus = 10,

    //Server Packets
    eSPacketHello = -1,
    eSPacketServerListReply = -2,
    eSPacketJoinServerReply = -3,
    eSPacketHostServerReply = -4,
    eSPacketUpdateLobbyData = -5,
    eSPacketUpdateLobbyChart = -6,
    eSPacketStartLobbyGame = -7,
    eSPacketUpdateLeaderboard = -8,
    eSPacketPlayerSongFinished = -9,
    eSPacketFinalizeChart = -10,


    //Other Server Packets
    eSPacketInvalidPacket = -91,
    eSPacketStatus = -42,
};

class Packet
{
public:
    int PacketType;
    long Order;
    MSGPACK_DEFINE_MAP(PacketType, Order);
};