/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_STEAM_H
#define AVG4K_F_STEAM_H

#pragma once

#include <string>
#include <vector>
#include "FData.h"

namespace Average4k::Api::Functions
{
	class FSteam {
	public:
		/// <summary>
		/// If steam is running
		/// </summary>
		/// <returns>If steam is running</returns>
		static bool IsSteamRunning();
		/// <summary>
		/// Tell steam to create an item
		/// </summary>
		static void CreateItem();

		static bool ScanFolderForBad(std::string folder);
		
		/// <summary>
		/// Upload a pack to the workshop
		/// </summary>
		/// <param name="folder">The folder to upload</param>
		/// <param name="banner">The banner to upload (as a thumbnail)</param>
		/// <param name="title">The title to set to the item</param>
		/// <param name="description">The description to set to the item</param>
		/// <param name="tags">The tags to associate with the item</param>
		static void UploadPack(std::string folder, std::string banner, std::string title, std::string description, sol::table tags);

		/// <summary>
		/// Upload a noteskin to the workshop
		/// </summary>
		/// <param name="folder">The folder to upload</param>
		/// <param name="banner">The banner to upload (as a thumbnail)</param>
		/// <param name="title">The title to set to the item</param>
		/// <param name="description">The description to set to the item</param>
		/// <param name="previewPictures">The preview pictures to associate with the item</param>
		/// <param name="tags">The tags to associate with the item</param>
		static void UploadNoteskin(std::string folder, std::string banner, std::string title, std::string description, sol::table previewPictures, sol::table tags);

		/// <summary>
		/// Upload a theme to the workshop
		/// </summary>
		/// <param name="folder">The folder to upload</param>
		/// <param name="banner">The banner to upload (as a thumbnail)</param>
		/// <param name="title">The title to set to the item</param>
		/// <param name="description">The description to set to the item</param>
		/// <param name="previewPictures">The preview pictures to associate with the item</param>
		/// <param name="tags">The tags to associate with the item</param>
		static void UploadTheme(std::string folder, std::string banner, std::string title, std::string description, sol::table previewPictures, sol::table tags);

		/// <summary>
		/// Delete the current item
		/// </summary>
		static void DeleteItem();

		/// <summary>
		/// Get the current item progress (uploading)
		/// </summary>
		/// <returns>The current progress from 0 to 1</returns>
		static float GetCurrentItemProgress();

		/// <summary>
		/// Tell steam to open up the overlay and go to the workshop
		/// </summary>
		static void OpenWorkshop();

		/// <summary>
		/// Get the current steam username
		/// </summary>
		/// <returns>The username</returns>
		static std::string GetSteamName();
	};
}

#endif