#pragma once


enum PacketType
{
    //Client Packets
    eCPacketHello = 1,
    eCPacketServerList = 2,
    eCPacketJoinServer = 3,
    eCPacketHostServer = 4,
    eCPacketHostChangeChart = 5,
    eCPacketClientChartAcquired = 6,
    eCPacketHostStartGame = 7,
    eCPacketNoteHit = 8,
    eCPacketSongFinished = 9,
    eCPacketStatus = 10,
    eCPacketWtfAmIn = 11,
    eCPacketLeave = 12,
    eCPacketHostUpdateLobby = 13,
    eCPacketHostEndChart = 14,
    eCPacketRequestAvatar = 15,
    eCPacketSendMessage = 16,

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
    eSPacketWtfAmInReply = -11,
    eSPacketAvatarRequestResponse = -12,
    eSPacketOnChat = -13,
    eSPacketFuckYou = -14,
    eSPacketUpdateEncryptionParameters = 15,


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