#pragma once
#include "Packet.h"
#include "includes.h"
class CPacketHostChangeChart : public Packet
{
public:
	std::string chartID;

	MSGPACK_DEFINE_MAP(chartID);
};