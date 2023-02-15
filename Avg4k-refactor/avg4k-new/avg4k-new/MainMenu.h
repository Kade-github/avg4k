#pragma once
#include "includes.h"
#include "Menu.h"

class MainMenu :
    public AvgEngine::Base::Menu
{
public:
	MainMenu();

	void load()	override;

	void draw() override;
};

