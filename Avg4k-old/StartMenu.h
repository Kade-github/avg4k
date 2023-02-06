#pragma once
#include "Menu.h"
#include "includes.h"
#include "AvgSprite.h"

class StartMenu : public Menu
{
public:

	AvgSprite* opgl;
	AvgSprite* kadedev;

	void create() override;

	void update(Events::updateEvent ev) override;

	void keyDown(SDL_KeyboardEvent event) override;
};

