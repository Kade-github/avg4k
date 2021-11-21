#pragma once
#include "includes.h"
class Menu
{
public:
	Menu() {};

	virtual void update(Events::updateEvent ev) = 0;
	virtual void keyDown(SDL_KeyboardEvent ev) {};
	virtual void keyUp(SDL_KeyboardEvent ev) {};
};