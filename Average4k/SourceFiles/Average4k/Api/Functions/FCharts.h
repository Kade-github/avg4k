/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_CHARTS_H
#define AVG4K_F_CHARTS_H

#pragma once

#include <string>

#include "../AvgLuaFile.h"
#include "../Stubs/LuaSong.h"
#include "../Stubs/LuaSprite.h"

namespace Average4k::Api::Functions
{
	class FCharts {
	public:
		static Average4k::Api::AvgLuaFile* Lua;

		static void ScanCharts();
		static sol::table GetPacks();
		static sol::table GetCharts(std::wstring pack);
		static void LoadChart(std::wstring path);
		static int LoadAsyncTexture(std::wstring path);
		static void LoadAsyncAudio(std::string name, std::string path);
		static void ClearAudio();
		static Average4k::Api::Stubs::LuaSong GetAsyncAudio();
		static Average4k::Api::Stubs::LuaSprite GetAsyncTexture(int id);
		static sol::table CheckChart();
		static void ClearAsync();

	};
}

#endif