/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FSkins.h"

#include <filesystem>

sol::table Average4k::Api::Functions::FSkins::GetSkins()
{
    sol::table skins = sol::table();

    for (const auto& entry : std::filesystem::directory_iterator("Assets/Skins"))
    {
		if (entry.is_directory())
		{
			skins.add(entry.path().filename().wstring());
		}
    }

    return skins;
}
