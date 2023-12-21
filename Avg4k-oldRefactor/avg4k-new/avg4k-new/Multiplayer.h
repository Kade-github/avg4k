#pragma once

#include "includes.h"
#include "Base64.h"
#include <iostream>
#include <boost_static/lockfree/queue.hpp>

#include "Game.h"
#include "Packet.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

struct PacketData {
	std::string data;
	PacketType packetType;
	int attempts;
};

struct QueuedPacket {
	msgpack::v2::object obj;
	int type;
};

class Multiplayer
{
public:
	static Multiplayer* MInstance;
	static bool connectedToServer;
	static boost::lockfree::queue<PacketData*> sendQueue;
	static void connect(AvgEngine::Game* g);

	static std::string currentUserAvatar;
	
	static std::string username;
	static std::string tag;

	static bool integ;
	static bool prot;
	static bool debugger;
	static bool vm;
	static bool speed;


	static int DetectSpeedhack(std::vector<char>* function);

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