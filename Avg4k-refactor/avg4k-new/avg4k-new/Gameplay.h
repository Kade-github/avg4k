#pragma once
#include "Game.h"
#include "includes.h"
#include "Menu.h"
#include "Container.h"
#include "GameplayMenu.h"

class Gameplay : public Average4k::Lua::GameplayMenu
{
public:
	std::vector<Sprite> receptors;
	std::vector<Sprite> notes;

	Gameplay(std::string path) : Average4k::Lua::GameplayMenu(path)
	{
	}

	void CreateArrows();

	void Reload() override;

	void load() override;

	void draw() override;
};
