/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FSkins.h"

#include <filesystem>

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

	return noteskins;
}
