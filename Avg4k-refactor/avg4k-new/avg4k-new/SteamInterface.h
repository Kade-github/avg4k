#pragma once
#include "includes.h"
#include <steam/steam_api.h>

namespace Steam
{
	struct CSteamUser
	{
		std::string id64;
		std::string nickname;
	};

	class SteamInterface
	{
	public:
		CSteamUser self{};

		bool good = false;

		SteamInterface()
		{
			good = SteamAPI_Init();
			if (!good)
				AvgEngine::Logging::writeLog("[ERROR] [Steam] Failed to initialize steam.");
			else
			{
				CSteamID id = SteamUser()->GetSteamID();
				const char* name = SteamFriends()->GetPersonaName();
				self = {std::to_string(id.ConvertToUint64()), std::string(name)};
				AvgEngine::Logging::writeLog("[Steam] Logged in as " + self.nickname + ":" + self.id64);
			}
		}

		void SetPresence(std::string presence)
		{
			SteamFriends()->ClearRichPresence();
			SteamFriends()->SetRichPresence("gamestatus", presence.c_str());
			SteamFriends()->SetRichPresence("steam_display", "#Status");
		}

		void destroy()
		{
			SteamAPI_Shutdown();
		}
	};
}