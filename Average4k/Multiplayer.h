#pragma once

#include "includes.h"
#include "CPacketHello.h"
#include "SPacketStatus.h"
#include "SPacketHello.h"
#include "CPacketStatus.h"
#include "Base64.h"
#include <iostream>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

struct PacketData {
	std::string data;
	PacketType packetType;
};

class Multiplayer
{
public:
	static bool connectedToServer;
	static std::queue<PacketData> sendQueue;
	static std::mutex sendQueueLock;
	static DWORD WINAPI connect(LPVOID agh);

	static void InitCrypto();

	static void SendPacket(std::string data, PacketType packet);

	template<typename T>
	static void sendMessage(T p)
	{
		std::stringstream stream;

		msgpack::pack(stream, p);

		websocketpp::lib::error_code ec;

		std::string dataStr = stream.str();

		SendPacket(dataStr, (PacketType)p.PacketType);
	}

private:
	void OnSteamAuthTicket(EncryptedAppTicketResponse_t* pEncryptedAppTicketResponse, bool bIOFailure);
	CCallResult<Multiplayer, EncryptedAppTicketResponse_t> steamEncryptedAppTicketCall;
public:
	void inQuotesGracefullyDisconnect();

	static bool loggedIn;

	void login();
};

class ConnectionData {
public:
	client c;
	websocketpp::connection_hdl connectionHdl;
};