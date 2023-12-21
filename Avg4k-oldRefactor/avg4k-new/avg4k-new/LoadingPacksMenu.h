#pragma once
#include "LuaMenu.h"
#include "SongGatherer.h"
using namespace AvgEngine::Base;

class LoadingPacksMenu : public Average4k::Lua::LuaMenu
{
public:
	Average4k::Chart::Collection::SongGatherer gather{};

	LoadingPacksMenu()
	{

	}
	void load() override;

	void draw() override;
};