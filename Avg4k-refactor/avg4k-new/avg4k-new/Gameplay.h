#pragma once
#include "Game.h"
#include "includes.h"
#include "Menu.h"
#include "Container.h"
#include "GameplayMenu.h"
#include "Playfield.h"
#include "RhythmChannel.h"
class Gameplay : public Average4k::Lua::GameplayMenu
{
public:
	Average4k::Audio::RhythmChannel* c = NULL;
	float startTimestamp = 0;
	float songStart = 0;

	float menuStartimestamp = 0;

	std::vector<Average4k::Objects::Gameplay::Playfield*> playfields{};

	Gameplay(std::string path) : Average4k::Lua::GameplayMenu(path)
	{
	}

	void AddPlayfield();

	void Reload() override;

	void load() override;

	void draw() override;
};
