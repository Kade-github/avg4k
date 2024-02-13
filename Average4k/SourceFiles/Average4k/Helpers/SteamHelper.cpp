/*
	Avg Dev Team
	2021-2024 Average4k
*/


#include "SteamHelper.h"
#include <steam/steam_api.h>

#include <AvgEngine/Utils/Logging.h>

bool Average4k::Helpers::SteamHelper::IsSteamRunning = false;

std::string Average4k::Helpers::SteamHelper::Id64 = "";
std::string Average4k::Helpers::SteamHelper::Nickname = "Not logged in";

void Average4k::Helpers::SteamHelper::Initialize()
{
	IsSteamRunning = SteamAPI_Init();

	if (!IsSteamRunning)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Could not initialize SteamAPI.");
		return;
	}

	AvgEngine::Logging::writeLog("[Steam] SteamAPI initialized.");

	CSteamID id = SteamUser()->GetSteamID();
	const char* name = SteamFriends()->GetPersonaName();

	AvgEngine::Logging::writeLog("[Steam] User:");
	AvgEngine::Logging::writeLog("[Steam] SteamID: " + std::to_string(id.ConvertToUint64()));
	AvgEngine::Logging::writeLog("[Steam] PersonaName: " + std::string(name));

	Id64 = id.ConvertToUint64();
	Nickname = std::string(name);

}

void Average4k::Helpers::SteamHelper::SetPresence(std::string presence)
{
	if (!IsSteamRunning)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Failed to set presence. SteamAPI is not running.");
		return;
	}

	SteamFriends()->SetRichPresence("gamestatus", presence.c_str());
	SteamFriends()->SetRichPresence("steam_display", "#Status");
	AvgEngine::Logging::writeLog("[Steam] Presence set to: " + presence);
}

void Average4k::Helpers::SteamHelper::Destroy()
{
	if (!IsSteamRunning)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Could not destroy SteamAPI. SteamAPI is not running.");
		return;
	}

	SteamAPI_Shutdown();
	AvgEngine::Logging::writeLog("[Steam] SteamAPI destroyed.");
}
