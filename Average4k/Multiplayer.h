#pragma once
#include <openssl/opensslconf.h>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <msgpack.hpp>

#include "Packet.h"

#include "includes.h"
#include "CPacketHello.h"
#include "SPacketStatus.h"
#include "SPacketHello.h"
#include "CPacketStatus.h"
#include "Base64.h"
#include <iostream>

class Multiplayer
{
public:
	static bool connectedToServer;

	static DWORD WINAPI connect(LPVOID agh);

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

	bool loggedIn;

	void login();
};

