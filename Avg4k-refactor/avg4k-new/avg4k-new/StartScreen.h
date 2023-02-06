#pragma once
#include "Game.h"
#include "includes.h"
#include "Menu.h"
using namespace AvgEngine::Base;

class StartScreen : public Menu
{
public:
	StartScreen();

	void load()	override;
};