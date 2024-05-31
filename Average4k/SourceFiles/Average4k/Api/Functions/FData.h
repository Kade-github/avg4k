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
		/// <summary>
		/// Get the skin save data
		/// </summary>
		/// <returns>The skin data as a table</returns>
		static sol::table GetSkinData();
		/// <summary>
		/// Set the skin save data
		/// </summary>
		/// <param name="data">The table to copy from</param>
		static void SetSkinData(sol::table data);
		/// <summary>
		/// Get the keybind save data
		/// </summary>
		/// <returns>The keybind data as a table</returns>
		static sol::table GetKeybindData();
		/// <summary>
		/// Set the keybind save data
		/// </summary>
		/// <param name="data">The table to copy from</param>
		static void SetKeybindData(sol::table data);
		/// <summary>
		/// Get the gameplay save data
		/// </summary>
		/// <returns>The gameplay data as a table</returns>
		static sol::table GetGameplayData();
		/// <summary>
		/// Set the gameplay save data
		/// </summary>
		/// <param name="data">The table to copy from</param>
		static void SetGameplayData(sol::table data);
		/// <summary>
		/// Get the video save data
		/// </summary>
		/// <returns>The video data as a table</returns>
		static sol::table GetVideoData();
		/// <summary>
		/// Set the video save data
		/// </summary>
		/// <param name="data">The table to copy from</param>
		static void SetVideoData(sol::table data);
		/// <summary>
		/// Get the audio save data
		/// </summary>
		/// <returns>The audio data as a table</returns>
		static sol::table GetAudioData();
		/// <summary>
		/// Set the audio save data
		/// </summary>
		/// <param name="data">The table to copy from</param>
		static void SetAudioData(sol::table data);

		/// <summary>
		/// Set the skin
		/// </summary>
		/// <param name="skinName">The name of the skin</param>
		static void SetSkin(std::string skinName);
	};
}

#endif