#pragma once

#ifndef CPACKETLEADERBOARDREQUEST_H
#define CPACKETLEADERBOARDREQUEST_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{

	class CPacketLeaderboardRequest : public Packet
	{
	public:
		unsigned long chartId;
		int diffIndex;

		long chartIndex;

		MSGPACK_DEFINE_MAP(chartId, diffIndex, chartIndex);
	};
}

#endif