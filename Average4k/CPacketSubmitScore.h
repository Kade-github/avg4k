#pragma once
#include "Packet.h"
class CPacketSubmitScore : public Packet
{
public:
	std::vector<float> timings;

	long chartIndex;

	unsigned long ChartId;

	MSGPACK_DEFINE_MAP(timings, chartIndex, ChartId);
};