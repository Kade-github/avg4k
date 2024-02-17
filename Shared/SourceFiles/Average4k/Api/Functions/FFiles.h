/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_FILES_H
#define AVG4K_F_FILES_H

#pragma once

#include <string>

namespace Average4k::Api::Functions
{
	class FFiles {
	public:
		/// <summary>
		/// Get a file from the user
		/// </summary>
		/// <param name="filters">The filters for the file dialog</param>
		/// <returns>The file path</returns>
		static std::string GetFileFromUser(std::string filters);
		/// <summary>
		/// Get a folder from the user
		/// </summary>
		/// <returns>The folder path</returns>
		static std::string GetFolderFromUser();
	};
}

#endif