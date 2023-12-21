#pragma once
#include "Game.h"
#include "includes.h"
#include "Menu.h"
#include "RhythmChannel.h"
#include "SkinUtils.h"
#include "Sprite.h"
#include "LuaMenu.h"
using namespace AvgEngine::Base;

class StartScreen : public Average4k::Lua::LuaMenu
{
public:
	bool startedT = false;
	bool started = false;
	double _eBeat = 0;
	double _tBeat = 0;
	double bopBeat = 0;
	double bpm = 0;
	double beatOffset = 0;
	Sprite* bg = NULL;
	Sprite* bump = NULL;
	Sprite* logo = NULL;
	Sprite* whs = NULL;
	Average4k::Audio::RhythmChannel* c = NULL;
	StartScreen();

	void load()	override;

	void draw() override;
};