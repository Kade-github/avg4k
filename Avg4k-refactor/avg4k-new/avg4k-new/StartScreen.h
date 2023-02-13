#pragma once
#include "Game.h"
#include "includes.h"
#include "Menu.h"
#include "RhythmChannel.h"
#include "SkinUtils.h"
using namespace AvgEngine::Base;

class StartScreen : public Menu
{
public:
	Average4k::Audio::RhythmChannel* c = NULL;
	StartScreen();

	void load()	override;

	void draw() override;
};