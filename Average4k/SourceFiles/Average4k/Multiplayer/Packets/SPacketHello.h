#pragma once

#ifndef SPACKETHELLO_H
#define SPACKETHELLO_H

#include "Packet.h"
#include "SPacketOnChat.h"

namespace Average4k::Multiplayer::Packets
{

	class SPacketHello : public Packet
	{
	public:
		std::string Message;

		bool Error;

		int ErrorCode;

		std::string reauth;
		std::string avatarURL;
		std::string Avatar;

		std::string chatTag;

		ccolor tagColor;
		ccolor nameColor;

		MSGPACK_DEFINE_MAP(Message, Error, ErrorCode, reauth, avatarURL, Avatar, chatTag, tagColor, nameColor);
	};
}

#endif