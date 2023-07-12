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
	Average4k::Audio::RhythmChannel* boom = NULL;
	float startTimestamp = 0;
	float songStart = 0;
	float songOffset = 0;
	float endTime = -1;
	bool downscroll = false;
	float menuStartimestamp = 0;

	bool failedCheck = false;

	std::vector<Average4k::Objects::Gameplay::Playfield*> playfields{};

	Gameplay(std::string path) : Average4k::Lua::GameplayMenu(path)
	{
	}
	void End();

	void AddPlayfield();

	void Reload() override;

	void load() override;

	void draw() override;
};
