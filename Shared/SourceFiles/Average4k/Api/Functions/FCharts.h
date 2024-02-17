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
		static AvgLuaFile* Lua;

		/// <summary>
		/// Scan for charts asynchronously
		/// </summary>
		static void ScanCharts();
		/// <summary>
		/// Get packs from scanned charts.
		/// </summary>
		/// <returns>A table full of packs</returns>
		static sol::table GetPacks();
		/// <summary>
		/// Get charts from a pack
		/// </summary>
		/// <param name="pack">The pack to get from</param>
		/// <returns>The charts</returns>
		static sol::table GetCharts(std::wstring pack);
		/// <summary>
		/// Load a chart asynchronously
		/// </summary>
		/// <param name="path">The chart path to load</param>
		static void LoadChart(std::wstring path);
		/// <summary>
		/// Load a texture asynchronously
		/// </summary>
		/// <param name="path">The texture's path</param>
		/// <returns>The id of the async texture</returns>
		static int LoadAsyncTexture(std::wstring path);
		/// <summary>
		/// Load an audio asynchronously
		/// </summary>
		/// <param name="name">The channel name of the loaded audio</param>
		/// <param name="path">The path of the audio to load</param>
		static void LoadAsyncAudio(std::string name, std::string path);
		/// <summary>
		/// Stop all channels, and remove them.
		/// </summary>
		static void ClearAudio();
		/// <summary>
		/// Get the current async audio
		/// </summary>
		/// <returns>The channel (if loaded)</returns>
		static Average4k::Api::Stubs::LuaSong GetAsyncAudio();
		/// <summary>
		/// Get an async texture from an id
		/// </summary>
		/// <param name="id">The id to look for</param>
		/// <returns>The texture (if loaded)</returns>
		static Average4k::Api::Stubs::LuaSprite GetAsyncTexture(int id);
		/// <summary>
		/// Get the current chart
		/// </summary>
		/// <returns>The chart (if loaded)</returns>
		static sol::table CheckChart();
		/// <summary>
		/// Clear all async data
		/// </summary>
		static void ClearAsync();

	};
}

#endif