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
		/// <summary>
		/// Quits the game
		/// </summary>
		static void Quit();
		/// <summary>
		/// Get the current width scale
		/// </summary>
		static float GetWidthScale();
		/// <summary>
		/// Get the current height scale
		/// </summary>
		static float GetHeightScale();
		/// <summary>
		/// Tell the game to fade out songs when switching menus
		/// </summary>
		/// <param name="fade">If the game should fade out songs</param>
		static void FadeOutSong(bool fade);
		/// <summary>
		/// Select a chart for play
		/// </summary>
		/// <param name="path">The path of the chart</param>
		/// <param name="diff">The difficulty to play</param>
		static void PlayChart(std::wstring path, int diff);
	};
}

#endif