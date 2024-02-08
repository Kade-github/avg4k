/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_GAME_H
#define AVG4K_F_GAME_H

#pragma once

#include <string>

namespace Average4k::Api::Functions
{
	class FGame {
	public:
		static void Quit();
		static float GetWidthScale();
		static float GetHeightScale();
		static void FadeOutSong(bool fade);
		static void PlayChart(std::wstring path, int diff);
	};
}

#endif