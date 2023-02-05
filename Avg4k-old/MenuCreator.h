#pragma once
#include "includes.h"
#include "Menu.h"
#include <SDL_draw.h>

class MenuCreator : public Menu
{
public:
	MenuCreator();

	void update(Events::updateEvent event) override;
	void keyDown(SDL_KeyboardEvent event) override;

};

