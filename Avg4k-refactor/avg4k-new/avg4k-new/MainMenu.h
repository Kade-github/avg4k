#pragma once
#include "includes.h"
#include "Menu.h"
#include "Container.h"
class MainMenu :
    public AvgEngine::Base::Menu
{
public:

	Container* soloContainer;
	

	MainMenu();

	void load()	override;

	void draw() override;
};

