#pragma once
#include "pch.h"

bool patched = false;

class Events
{
	public:
		struct updateEvent {
			SDL_Renderer* renderer;
			SDL_Window* window;
		};

		struct packetEvent
		{
			PacketType type;
			char* data;
			int32_t length;
			char* ogPtr;
		};


};