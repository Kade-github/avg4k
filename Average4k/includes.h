#pragma once
#include "Packet.h"
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "steam/steam_api.h"
#include <Themida/ThemidaSDK.h>


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