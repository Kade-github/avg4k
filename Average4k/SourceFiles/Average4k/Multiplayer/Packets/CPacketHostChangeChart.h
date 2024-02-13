#pragma once

#ifndef CPACKETHOSTCHANGECHART_H
#define CPACKETHOSTCHANGECHART_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{
	class CPacketHostChangeChart : public Packet
	{
	public:
		bool isPack;
		uint64_t packID;
		uint64_t chartID;
		int chartIndex;
		int diff;
		MSGPACK_DEFINE_MAP(isPack, packID, chartID, chartIndex, diff);
	};
}
#endif