/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_STEAM_HELPER_H
#define AVG4K_STEAM_HELPER_H

#pragma once

#include <string>


namespace Average4k::Helpers
{
	class SteamHelper
	{
	public:
		static std::string Id64;
		static std::string Nickname;
		static bool IsSteamRunning;

		static void Initialize();

		static void SetPresence(std::string presence);

		static void Destroy();
	};
}

#endif