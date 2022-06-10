#pragma once
#include "Packet.h"
class CPacketSubmitScore : public Packet
{
public:
	std::vector<float> noteTiming;

	long chartIndex;

	unsigned long ChartId;

	MSGPACK_DEFINE_MAP(noteTiming, chartIndex, ChartId);
};