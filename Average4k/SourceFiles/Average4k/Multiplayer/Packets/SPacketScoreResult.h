#pragma once

#ifndef SPACKETSCORERESULT_H
#define SPACKETSCORERESULT_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{

	class SPacketScoreResult : public Packet
	{
	public:
		bool ok;

		std::string error;

		MSGPACK_DEFINE_MAP(ok, error);
	};
}

#endif