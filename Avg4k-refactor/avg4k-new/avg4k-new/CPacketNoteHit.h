#pragma once
#include "Packet.h"
#include "includes.h"
class CPacketNoteHit : public Packet
{
	public:
		unsigned long NoteID;
		float NoteTiming;

		MSGPACK_DEFINE_MAP(NoteID, NoteTiming);
};