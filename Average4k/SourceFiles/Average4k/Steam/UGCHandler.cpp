#include "UGCHandler.h"

#include <AvgEngine/Utils/Logging.h>

Average4k::Steam::UGCHandler* Average4k::Steam::UGCHandler::Instance = NULL;

float Average4k::Steam::UGCHandler::GetCurrentItemProgress()
{
	if (currentItem_updateHandle != 0)
	{
		uint64_t totalBytes = 0;
		uint64_t bytesProcessed = 0;
		SteamUGC()->GetItemUpdateProgress(currentItem_updateHandle, &bytesProcessed, &totalBytes);
		float prog = 0.0f;
		if (totalBytes != 0)
			prog = (float)bytesProcessed / (float)totalBytes;
		if (prog >= 1.0f)
		{
			if (isDone)
				return 1.0f;
			else
				return 0.95f;
		}
	}
	return 0.0f;
}

void Average4k::Steam::UGCHandler::onItemCreated(CreateItemResult_t* pCallback, bool bIOFailure)
{
	if (bIOFailure)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Failed to create item. Error: " + std::to_string(pCallback->m_eResult));
		DeleteItem();
		return;
	}

	isDone = false;

	if (pCallback->m_bUserNeedsToAcceptWorkshopLegalAgreement)
	{
		SteamFriends()->ActivateGameOverlayToWebPage("steam://url/CommunityFilePage/");
	}

	AvgEngine::Logging::writeLog("[Steam] Item created. ID: " + std::to_string(pCallback->m_nPublishedFileId));
	currentItem = pCallback->m_nPublishedFileId;
}

void Average4k::Steam::UGCHandler::onItemSubmitted(SubmitItemUpdateResult_t* pCallback, bool bIOFailure)
{
	if (bIOFailure)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Failed to submit item update. Error: " + std::to_string(pCallback->m_eResult));
		DeleteItem();
		return;
	}
	isDone = true;
	AvgEngine::Logging::writeLog("[Steam] Item submitted. ID: " + std::to_string(pCallback->m_nPublishedFileId));

	currentItem = {};
}

void Average4k::Steam::UGCHandler::onItemDeleted(DeleteItemResult_t* pCallback, bool bIOFailure)
{
	if (bIOFailure)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Failed to delete item. Error: " + std::to_string(pCallback->m_eResult));
		return;
	}

	AvgEngine::Logging::writeLog("[Steam] Item deleted. ID: " + std::to_string(pCallback->m_nPublishedFileId));

	currentItem = {};
}

void Average4k::Steam::UGCHandler::UploadPack(std::string folder, std::string previewPath, std::string title, std::string description, std::vector<std::string> tags)
{
	if (currentItem_updateHandle != 0)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Item update already in progress.");
		return;
	}

	currentItem_updateHandle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), currentItem);

	SteamUGC()->SetItemTitle(currentItem_updateHandle, title.c_str());
	SteamUGC()->SetItemDescription(currentItem_updateHandle, description.c_str());

	SteamUGC()->SetItemContent(currentItem_updateHandle, folder.c_str());

	SteamUGC()->SetItemPreview(currentItem_updateHandle, previewPath.c_str());
	SteamUGC()->SetItemVisibility(currentItem_updateHandle, k_ERemoteStoragePublishedFileVisibilityPublic);

	SteamParamStringArray_t paramArray;

	paramArray.m_nNumStrings = tags.size();
	paramArray.m_ppStrings = new const char* [tags.size()];
	for (int i = 0; i < tags.size(); i++)
	{
		paramArray.m_ppStrings[i] = tags[i].c_str();
	}

	SteamUGC()->SetItemTags(currentItem_updateHandle, &paramArray);

	SteamAPICall_t hSteamAPICall = SteamUGC()->SubmitItemUpdate(currentItem_updateHandle, "Initial upload");
	m_SubmitItemUpdateResult.Set(hSteamAPICall, this, &UGCHandler::onItemSubmitted);
}

void Average4k::Steam::UGCHandler::UploadNoteskin(std::string folder, std::string previewPath, std::string title, std::string description, std::vector<std::string> previewPictures, std::vector<std::string> tags)
{
	if (currentItem_updateHandle != 0)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Item update already in progress.");
		return;
	}

	currentItem_updateHandle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), currentItem);

	SteamUGC()->SetItemTitle(currentItem_updateHandle, title.c_str());
	SteamUGC()->SetItemDescription(currentItem_updateHandle, description.c_str());

	SteamUGC()->SetItemContent(currentItem_updateHandle, folder.c_str());

	SteamUGC()->SetItemPreview(currentItem_updateHandle, previewPath.c_str());
	SteamUGC()->SetItemVisibility(currentItem_updateHandle, k_ERemoteStoragePublishedFileVisibilityPublic);

	for (std::string s : previewPictures)
	{
		SteamUGC()->AddItemPreviewFile(currentItem_updateHandle, s.c_str(), EItemPreviewType::k_EItemPreviewType_Image);
	}

	SteamParamStringArray_t paramArray;

	paramArray.m_nNumStrings = tags.size();
	paramArray.m_ppStrings = new const char* [tags.size()];
	for (int i = 0; i < tags.size(); i++)
	{
		paramArray.m_ppStrings[i] = tags[i].c_str();
	}

	SteamUGC()->SetItemTags(currentItem_updateHandle, &paramArray);

	SteamAPICall_t hSteamAPICall = SteamUGC()->SubmitItemUpdate(currentItem_updateHandle, "Initial upload");
	m_SubmitItemUpdateResult.Set(hSteamAPICall, this, &UGCHandler::onItemSubmitted);
}

void Average4k::Steam::UGCHandler::UploadTheme(std::string folder, std::string previewPath, std::string title, std::string description, std::vector<std::string> previewPictures, std::vector<std::string> tags)
{
	if (currentItem_updateHandle != 0)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Item update already in progress.");
		return;
	}

	currentItem_updateHandle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), currentItem);

	SteamUGC()->SetItemTitle(currentItem_updateHandle, title.c_str());
	SteamUGC()->SetItemDescription(currentItem_updateHandle, description.c_str());

	SteamUGC()->SetItemContent(currentItem_updateHandle, folder.c_str());

	SteamUGC()->SetItemPreview(currentItem_updateHandle, previewPath.c_str());
	SteamUGC()->SetItemVisibility(currentItem_updateHandle, k_ERemoteStoragePublishedFileVisibilityPublic);

	for (std::string s : previewPictures)
	{
		SteamUGC()->AddItemPreviewFile(currentItem_updateHandle, s.c_str(), EItemPreviewType::k_EItemPreviewType_Image);
	}

	SteamParamStringArray_t paramArray;

	paramArray.m_nNumStrings = tags.size();

	paramArray.m_ppStrings = new const char* [tags.size()];

	for (int i = 0; i < tags.size(); i++)
	{
		paramArray.m_ppStrings[i] = tags[i].c_str();
	}

	SteamUGC()->SetItemTags(currentItem_updateHandle, &paramArray);

	SteamAPICall_t hSteamAPICall = SteamUGC()->SubmitItemUpdate(currentItem_updateHandle, "Initial upload");
	m_SubmitItemUpdateResult.Set(hSteamAPICall, this, &UGCHandler::onItemSubmitted);
}

void Average4k::Steam::UGCHandler::CreateItem()
{
	static bool isDeleting = false;
	if (currentItem != 0)
	{
		if (!isDeleting)
			DeleteItem();
		isDeleting = true;
		AvgEngine::Logging::writeLog("[Steam] [Error] Item already exists, deleting. Please retry.");
		return;
	}
	isDeleting = false;

	SteamAPICall_t hSteamAPICall = SteamUGC()->CreateItem(SteamUtils()->GetAppID(), k_EWorkshopFileTypeCommunity);
	m_CreateItemResult.Set(hSteamAPICall, this, &UGCHandler::onItemCreated);
}

void Average4k::Steam::UGCHandler::DeleteItem()
{
	if (currentItem == 0)
		return;

	SteamAPICall_t hSteamAPICall = SteamUGC()->DeleteItem(currentItem);
	m_DeleteItemResult.Set(hSteamAPICall, this, &UGCHandler::onItemDeleted);
}