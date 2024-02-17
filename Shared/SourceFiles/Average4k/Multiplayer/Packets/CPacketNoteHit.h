#pragma once

#ifndef CPACKETNOTEHIT_H
#define CPACKETNOTEHIT_H

#include "Packet.h"

namespace Average4k::Multiplayer::Packets
{

	class CPacketNoteHit : public Packet
	{
	public:
		unsigned long NoteID;
		float NoteTiming;

		MSGPACK_DEFINE_MAP(NoteID, NoteTiming);
	};
}

#endif