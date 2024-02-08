/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_GAMEPLAY_H
#define AVG4K_GAMEPLAY_H

#pragma once
#include <AvgEngine/Base/Menu.h>
#include "../../Data/Chart/Providers/StepFile.h"
#include "../../A4kGame.h"
#include "../../Api/AvgLuaFile.h"

namespace Average4k::Screens::Menu
{
	class Gameplay : public AvgEngine::Base::Menu
	{
		Average4k::Data::SaveData* save;
		std::wstring _path;
		int _diff;
	public:
		Average4k::Api::AvgLuaFile* lua;
		Average4k::Data::ChartFile chart;

		int noteWidth = 0;
		int noteHeight = 0;

		std::string comboTag = "";
		std::string judgementTextTag = "";
		std::map<std::string, std::string> judgementTags = {};

		Gameplay(std::wstring chart_path, int diff)
		{
			_path = chart_path;
			_diff = diff - 1; // - 1 cuz lua
		}

		void loadChart();
		void loadBackground();
		void loadPlayfield();

		void start();

		void load() override;
		void draw() override;
	};
}

#endif