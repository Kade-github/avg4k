#pragma once
#include "Game.h"
#include "includes.h"
#include "Menu.h"
#include "RhythmChannel.h"
#include "SkinUtils.h"
#include "Sprite.h"
using namespace AvgEngine::Base;

class StartScreen : public Menu
{
public:
	bool started = false;
	Sprite* bg = NULL;
	Sprite* logo = NULL;
	Average4k::Audio::RhythmChannel* c = NULL;
	StartScreen();

	void load()	override;

	void draw() override;
};