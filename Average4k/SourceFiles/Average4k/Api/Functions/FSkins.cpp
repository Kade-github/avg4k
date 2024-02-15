/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FSkins.h"

#include <filesystem>
#include "../../Steam/UGCHandler.h"
#include "../../Helpers/SteamHelper.h"
#include <AvgEngine/Utils/Paths.h>

sol::table Average4k::Api::Functions::FSkins::GetSkins()
{
	sol::table skins = FCharts::Lua->getState().create_table();

	std::string selected = A4kGame::gameInstance->saveData.skinData.name;

	bool contains = false;

    for (const auto& entry : std::filesystem::directory_iterator("Assets/Skins"))
    {
		if (entry.is_directory())
		{
			std::wstring name = entry.path().filename().wstring();

			if (name == std::wstring(selected.begin(), selected.end()))
				contains = true;

			skins.add(name);
		}
    }

	if (Helpers::SteamHelper::IsSteamRunning)
	{
		for (auto p : Steam::UGCHandler::Instance->subscribedThemes)
		{
			if (AvgEngine::Utils::Paths::pathExists(p.second))
			{
				if (p.first == selected)
					contains = true;
				skins.add(p.first);
			}
		}
	}

	if (!contains) // set to default
	{
		A4kGame::gameInstance->saveData.skinData = {};
	}

    return skins;
}

sol::table Average4k::Api::Functions::FSkins::GetNoteskins()
{
	sol::table noteskins = FCharts::Lua->getState().create_table();

	std::string selected = A4kGame::gameInstance->saveData.gameplayData.noteskin;

	bool contains = false;

	for (const auto& entry : std::filesystem::directory_iterator("Assets/Noteskin"))
	{
		if (entry.is_directory())
		{
			std::wstring name = entry.path().filename().wstring();

			if (name == std::wstring(selected.begin(), selected.end()))
				contains = true;

			noteskins.add(name);
		}
	}

	if (Helpers::SteamHelper::IsSteamRunning)
	{
		for (auto p : Steam::UGCHandler::Instance->subscribedNoteskins)
		{
			if (AvgEngine::Utils::Paths::pathExists(p.second))
			{
				if (p.first == selected)
					contains = true;
				noteskins.add(p.first);
			}
		}
	}

	if (!contains)
	{
		A4kGame::gameInstance->saveData.gameplayData.noteskin = "AArrow";
	}

	return noteskins;
}
