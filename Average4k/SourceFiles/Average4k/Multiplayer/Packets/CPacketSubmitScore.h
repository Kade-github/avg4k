#pragma once

#ifndef CPACKETSUBMITSCORE_H
#define CPACKETSUBMITSCORE_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{
	class CPacketSubmitScore : public Packet
	{
	public:
		// time, msDiff
		std::map<float, float> noteTiming;

		long long chartIndex;
		long long diffIndex;

		unsigned long long ChartId;

		std::string hash;

		MSGPACK_DEFINE_MAP(noteTiming, chartIndex, diffIndex, ChartId, hash);
	};
}

#endif