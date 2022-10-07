#pragma once
#include "Packet.h"
class CPacketSubmitScore : public Packet
{
public:
	// time, msDiff
	std::map<float, float> noteTiming;

	int combo;

	long chartIndex;
	long diffIndex;

	unsigned long ChartId;

	MSGPACK_DEFINE_MAP(chartIndex, ChartId);
};