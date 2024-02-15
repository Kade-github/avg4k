/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_STEAM_H
#define AVG4K_F_STEAM_H

#pragma once

#include <string>
#include <vector>

namespace Average4k::Api::Functions
{
	class FSteam {
	public:
		static bool IsSteamRunning();
		static void CreateItem();

		static bool ScanFolderForBad(std::string folder);

		static void UploadPack(std::string folder, std::string banner, std::string title, std::string description, std::vector<std::string> tags);
		static void UploadNoteskin(std::string folder, std::string banner, std::string title, std::string description, std::vector<std::string> previewPictures, std::vector<std::string> tags);
		static void UploadTheme(std::string folder, std::string banner, std::string title, std::string description, std::vector<std::string> previewPictures);

		static void DeleteItem();

		static void OpenWorkshop();
		static std::string GetSteamName();
	};
}

#endif