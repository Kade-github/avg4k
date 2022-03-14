#pragma once
#include "Menu.h"
#include "includes.h"
#include "AvgSprite.h"
#include "Text.h"
#include "TweenManager.h"

class MainerMenu : public Menu
{
public:
	float lastBeat = 0;
	bool started;
	AvgSprite* icon;
	AvgSprite* bg;
	Text* hello;
	Text* bottom;
	AvgSprite* border;

	void create() override;

	void update(Events::updateEvent ev) override;

	void keyDown(SDL_KeyboardEvent event) override;
};

