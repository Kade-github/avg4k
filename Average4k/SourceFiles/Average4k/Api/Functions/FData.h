/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_DATA_H
#define AVG4K_F_DATA_H

#pragma once

#include "FCharts.h"

namespace Average4k::Api::Functions
{
	class FData {
	public:
		static sol::table GetSkinData();
		static void SetSkinData(sol::table data);
		static sol::table GetKeybindData();
		static void SetKeybindData(sol::table data);
		static sol::table GetGameplayData();
		static void SetGameplayData(sol::table data);

		static void SetSkin(std::string skinName);
	};
}

#endif