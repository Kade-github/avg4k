/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FSkins.h"

#include <filesystem>
#include "../../Steam/UGCHandler.h"
#include "../../Helpers/SteamHelper.h"

sol::table Average4k::Api::Functions::FSkins::GetSkins()
{
	sol::table skins = FCharts::Lua->getState().create_table();

    for (const auto& entry : std::filesystem::directory_iterator("Assets/Skins"))
    {
		if (entry.is_directory())
		{
			std::wstring name = entry.path().filename().wstring();
			skins.add(name);
		}
    }

	if (Helpers::SteamHelper::IsSteamRunning)
	{
		for (auto p : Steam::UGCHandler::Instance->subscribedThemes)
		{
			skins.add(p.first);
		}
	}

    return skins;
}

sol::table Average4k::Api::Functions::FSkins::GetNoteskins()
{
	sol::table noteskins = FCharts::Lua->getState().create_table();

	for (const auto& entry : std::filesystem::directory_iterator("Assets/Noteskin"))
	{
		if (entry.is_directory())
		{
			std::wstring name = entry.path().filename().wstring();
			noteskins.add(name);
		}
	}

	if (Helpers::SteamHelper::IsSteamRunning)
	{
		for (auto p : Steam::UGCHandler::Instance->subscribedNoteskins)
		{
			noteskins.add(p.first);
		}
	}

	return noteskins;
}
