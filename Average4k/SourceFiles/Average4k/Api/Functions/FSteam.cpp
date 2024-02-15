#include "FSteam.h"
#include <AvgEngine/Utils/Logging.h>
#include "../../Helpers/SteamHelper.h"
#include "../../Steam/UGCHandler.h"

#include <filesystem>

bool Average4k::Api::Functions::FSteam::IsSteamRunning()
{
	return Average4k::Helpers::SteamHelper::IsSteamRunning;
}

void Average4k::Api::Functions::FSteam::CreateItem()
{
	Steam::UGCHandler::Instance->CreateItem();
}

bool Average4k::Api::Functions::FSteam::ScanFolderForBad(std::string folder)
{
	// recursive iterator

	for (const auto& entry : std::filesystem::recursive_directory_iterator(folder))
	{
		if (entry.path().extension() == ".exe" || entry.path().extension() == ".scr" || entry.path().extension() == ".bat" || entry.path().extension() == ".sh" || entry.path().extension() == ".ps" || entry.path().extension() == ".dll")
		{
			AvgEngine::Logging::writeLog("[Steam] [Error] Blacklisted file found: " + entry.path().string());
			return false;
		}
	}
	return true;
}

void Average4k::Api::Functions::FSteam::UploadPack(std::string folder, std::string banner, std::string title, std::string description, std::vector<std::string> tags)
{
	if (folder.empty())
	{
		MessageBoxW(NULL, L"Failed to upload pack, folder is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Folder is empty.");
		return;
	}

	if (banner.empty())
	{
		MessageBoxW(NULL, L"Failed to upload pack, banner is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Banner is empty.");
		return;
	}

	if (title.empty())
	{
		MessageBoxW(NULL, L"Failed to upload pack, title is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Title is empty.");
		return;
	}

	if (description.empty())
	{
		MessageBoxW(NULL, L"Failed to upload pack, description is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Description is empty.");
		return;
	}

	if (!ScanFolderForBad(folder))
	{
		MessageBoxW(NULL, L"Failed to upload pack, it contains blacklisted filetypes.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Pack folder contains blacklisted filetypes.");
		return;
	}

	// display message box telling user it's uploading stuff

	std::string s = "Uploading " + folder + " as a pack. Do you want to do this?";

	// Convert to a wchar_t*

	std::wstring wsTmp(s.begin(), s.end());
	// Display message box

	const int result = MessageBoxW(NULL, wsTmp.c_str(), L"Uploading", MB_YESNO);

	if (result == IDNO)
	{
		AvgEngine::Logging::writeLog("[Steam] User cancelled upload.");
		return;
	}

	if (Steam::UGCHandler::Instance->currentItem == 0)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] You must create an item before uploading it.");
		return;
	}
	Steam::UGCHandler::Instance->UploadPack(folder, banner, title, description, tags);
}

void Average4k::Api::Functions::FSteam::UploadNoteskin(std::string folder, std::string banner, std::string title, std::string description, std::vector<std::string> previewPictures, std::vector<std::string> tags)
{
	if (folder.empty())
	{
		MessageBoxW(NULL, L"Failed to upload noteskin, folder is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Folder is empty.");
		return;
	}

	if (banner.empty())
	{
		MessageBoxW(NULL, L"Failed to upload noteskin, banner is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Banner is empty.");
		return;
	}

	if (title.empty())
	{
		MessageBoxW(NULL, L"Failed to upload noteskin, title is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Title is empty.");
		return;
	}

	if (description.empty())
	{
		MessageBoxW(NULL, L"Failed to upload noteskin, description is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Description is empty.");
		return;
	}

	if (!ScanFolderForBad(folder))
	{
		MessageBoxW(NULL, L"Failed to upload noteskin, it contains blacklisted filetypes.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Noteskin folder contains blacklisted filetypes.");
		return;
	}

	// display message box telling user it's uploading stuff

	std::string s = "Uploading " + folder + " as a noteskin. Do you want to do this?";

	// Convert to a wchar_t*

	std::wstring wsTmp(s.begin(), s.end());
	// Display message box

	const int result = MessageBoxW(NULL, wsTmp.c_str(), L"Uploading", MB_YESNO);

	if (result == IDNO)
	{
		AvgEngine::Logging::writeLog("[Steam] User cancelled upload.");
		return;
	}

	if (Steam::UGCHandler::Instance->currentItem == 0)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] You must create an item before uploading it.");
		return;
	}

	Steam::UGCHandler::Instance->UploadNoteskin(folder, banner, title, description, previewPictures, tags);
}

void Average4k::Api::Functions::FSteam::UploadTheme(std::string folder, std::string banner, std::string title, std::string description, std::vector<std::string> previewPictures)
{
	if (folder.empty())
	{
		MessageBoxW(NULL, L"Failed to upload theme, folder is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Folder is empty.");
		return;
	}

	if (banner.empty())
	{
		MessageBoxW(NULL, L"Failed to upload theme, banner is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Banner is empty.");
		return;
	}

	if (title.empty())
	{
		MessageBoxW(NULL, L"Failed to upload theme, title is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Title is empty.");
		return;
	}

	if (description.empty())
	{
		MessageBoxW(NULL, L"Failed to upload theme, description is empty.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Description is empty.");
		return;
	}

	if (!ScanFolderForBad(folder))
	{
		MessageBoxW(NULL, L"Failed to upload theme, it contains blacklisted filetypes.", L"Failure to upload", MB_OK);
		AvgEngine::Logging::writeLog("[Steam] [Error] Theme folder contains blacklisted filetypes.");
		return;
	}

	// display message box telling user it's uploading stuff

	std::string s = "Uploading " + folder + " as a theme. Do you want to do this?";

	// Convert to a wchar_t*

	std::wstring wsTmp(s.begin(), s.end());
	// Display message box

	const int result = MessageBoxW(NULL, wsTmp.c_str(), L"Uploading", MB_YESNO);

	if (result == IDNO)
	{
		AvgEngine::Logging::writeLog("[Steam] User cancelled upload.");
		return;
	}

	if (Steam::UGCHandler::Instance->currentItem == 0)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] You must create an item before uploading it.");
		return;
	}

	Steam::UGCHandler::Instance->UploadTheme(folder, banner, title, description, previewPictures);
}

void Average4k::Api::Functions::FSteam::DeleteItem()
{
	Steam::UGCHandler::Instance->DeleteItem();
}

void Average4k::Api::Functions::FSteam::OpenWorkshop()
{
	SteamFriends()->ActivateGameOverlayToWebPage("https://steamcommunity.com/app/1828580/workshop/");
}

std::string Average4k::Api::Functions::FSteam::GetSteamName()
{
	return Average4k::Helpers::SteamHelper::Nickname;
}
