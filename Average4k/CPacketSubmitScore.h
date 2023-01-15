#pragma once
#include "Packet.h"
class CPacketSubmitScore : public Packet
{
public:
	// time, msDiff
	std::map<float, float> noteTiming;

	long long chartIndex;
	long long diffIndex;

	unsigned long long ChartId;

	std::string hash;

	MSGPACK_DEFINE_MAP(chartIndex, diffIndex, ChartId, hash);
};