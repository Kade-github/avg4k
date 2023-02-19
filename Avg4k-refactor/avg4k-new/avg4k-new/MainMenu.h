#pragma once
#include "Game.h"
#include "includes.h"
#include "Menu.h"
#include "Container.h"
class MainMenu :
    public AvgEngine::Base::Menu
{
public:
	AvgEngine::Base::Text* soloText = NULL;

	Container* soloContainer;
	

	MainMenu();

	void load()	override;

	void draw() override;
};

