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
	bool startedT = false;
	bool started = false;
	double _eBeat = 0;
	double _tBeat = 0;
	double bpm = 0;
	Sprite* bg = NULL;
	Sprite* bump = NULL;
	Sprite* logo = NULL;
	Sprite* whs = NULL;
	Average4k::Audio::RhythmChannel* c = NULL;
	StartScreen();

	void load()	override;

	void draw() override;
};