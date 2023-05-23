#pragma once
#include "Game.h"
#include "includes.h"
#include "Menu.h"
#include "Container.h"
#include "GameplayMenu.h"
#include "Playfield.h"

class Gameplay : public Average4k::Lua::GameplayMenu
{
public:

	std::vector<Average4k::Objects::Gameplay::Playfield*> playfields{};

	Gameplay(std::string path) : Average4k::Lua::GameplayMenu(path)
	{
	}


	void Reload() override;

	void load() override;

	void draw() override;
};
