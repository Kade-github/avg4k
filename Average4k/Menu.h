#pragma once
#include "includes.h"
#include "Packet.h"
class Menu
{
public:
	Menu() {};

	virtual void onPacket(PacketType pt, char* data, int32_t length) {};

	virtual void update(Events::updateEvent ev) = 0;
	virtual void keyDown(SDL_KeyboardEvent ev) {};
	virtual void keyUp(SDL_KeyboardEvent ev) {};
};