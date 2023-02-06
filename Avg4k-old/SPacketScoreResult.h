#pragma once
#include "Packet.h"
#include "includes.h"

class SPacketScoreResult : public Packet
{
public:
	bool ok;

	std::string error;

	MSGPACK_DEFINE_MAP(ok, error);
};