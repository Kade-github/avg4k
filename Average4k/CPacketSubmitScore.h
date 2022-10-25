#pragma once
#include "Packet.h"
class CPacketSubmitScore : public Packet
{
public:
	// time, msDiff
	std::map<float, float> noteTiming;

	long chartIndex;
	long diffIndex;

	unsigned long ChartId;

	std::string hash;

	MSGPACK_DEFINE_MAP(chartIndex, diffIndex, ChartId, hash);
};