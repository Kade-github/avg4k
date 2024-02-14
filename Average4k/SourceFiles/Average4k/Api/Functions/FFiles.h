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
		static std::string GetFileFromUser(std::string filters);
		static std::string GetFolderFromUser();
	};
}

#endif