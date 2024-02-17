/*
	Avg Dev Team
	2021-2024 Average4k
*/


#include "FFiles.h"
#include "../../Helpers/FileDiag.h"

std::string Average4k::Api::Functions::FFiles::GetFileFromUser(std::string f)
{
	std::wstring stemp = std::wstring(f.begin(), f.end());
	LPCWSTR sw = stemp.c_str();
	std::string file = Average4k::Helpers::Files::GetFile(sw);

	return file;
}

std::string Average4k::Api::Functions::FFiles::GetFolderFromUser()
{
	std::string folder = Average4k::Helpers::Files::GetFolder();

	return folder;
}
