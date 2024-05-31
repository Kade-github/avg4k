#pragma once

#ifndef _AVERAGE4K_M_CONNECTION_H
#define _AVERAGE4K_M_CONNECTION_H

#define _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING

#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#include <Themida/ThemidaSDK.h>
#include <iostream>
#include <openssl/opensslconf.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <steam/steam_api.h>
#include <boost_static/lockfree/queue.hpp>

#include "Packets/Packet.h"

namespace Average4k::Multiplayer {

	struct PacketData {
		std::string data;
		PacketType packetType;
		int attempts;
	};

	struct QueuedPacket {


		msgpack::v2::object obj;
		int type;
	};

	class Connection
	{
	public:
		static Connection* Instance;
		static bool connectedToServer;
		static boost::lockfree::queue<PacketData*> sendQueue;
		static void connect();

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
		static void sendMessage(T p);

	private:
		void OnSteamAuthTicket(EncryptedAppTicketResponse_t* pEncryptedAppTicketResponse, bool bIOFailure);
		CCallResult<Connection, EncryptedAppTicketResponse_t> steamEncryptedAppTicketCall;
	public:
		void inQuotesGracefullyDisconnect();

		static bool loggedIn;

		void login();
	};


}

#endif