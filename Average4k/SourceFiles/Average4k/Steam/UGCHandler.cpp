#include "UGCHandler.h"

#include <AvgEngine/Utils/Logging.h>

Average4k::Steam::UGCHandler* Average4k::Steam::UGCHandler::Instance = NULL;

void Average4k::Steam::UGCHandler::onItemCreated(CreateItemResult_t* pCallback, bool bIOFailure)
{
	if (bIOFailure)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Failed to create item. Error: " + std::to_string(pCallback->m_eResult));
		DeleteItem();
		return;
	}

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
	UGCUpdateHandle_t handle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), currentItem);

	SteamUGC()->SetItemTitle(handle, title.c_str());
	SteamUGC()->SetItemDescription(handle, description.c_str());

	SteamUGC()->SetItemContent(handle, folder.c_str());

	SteamUGC()->SetItemPreview(handle, previewPath.c_str());

	SteamParamStringArray_t paramArray;

	paramArray.m_nNumStrings = tags.size();
	paramArray.m_ppStrings = new const char* [tags.size()];
	for (int i = 0; i < tags.size(); i++)
	{
		paramArray.m_ppStrings[i] = tags[i].c_str();
	}

	SteamUGC()->SetItemTags(handle, &paramArray);

	SteamAPICall_t hSteamAPICall = SteamUGC()->SubmitItemUpdate(handle, "Initial upload");
	m_SubmitItemUpdateResult.Set(hSteamAPICall, this, &UGCHandler::onItemSubmitted);
}

void Average4k::Steam::UGCHandler::UploadNoteskin(std::string folder, std::string previewPath, std::string title, std::string description, std::vector<std::string> previewPictures, std::vector<std::string> tags)
{
	UGCUpdateHandle_t handle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), currentItem);

	SteamUGC()->SetItemTitle(handle, title.c_str());
	SteamUGC()->SetItemDescription(handle, description.c_str());

	SteamUGC()->SetItemContent(handle, folder.c_str());

	SteamUGC()->SetItemPreview(handle, previewPath.c_str());

	for (std::string s : previewPictures)
	{
		SteamUGC()->AddItemPreviewFile(handle, s.c_str(), EItemPreviewType::k_EItemPreviewType_Image);
	}

	SteamParamStringArray_t paramArray;

	paramArray.m_nNumStrings = tags.size();
	paramArray.m_ppStrings = new const char* [tags.size()];
	for (int i = 0; i < tags.size(); i++)
	{
		paramArray.m_ppStrings[i] = tags[i].c_str();
	}

	SteamUGC()->SetItemTags(handle, &paramArray);

	SteamAPICall_t hSteamAPICall = SteamUGC()->SubmitItemUpdate(handle, "Initial upload");
	m_SubmitItemUpdateResult.Set(hSteamAPICall, this, &UGCHandler::onItemSubmitted);
}

void Average4k::Steam::UGCHandler::UploadTheme(std::string folder, std::string previewPath, std::string title, std::string description, std::vector<std::string> previewPictures)
{
	UGCUpdateHandle_t handle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), currentItem);

	SteamUGC()->SetItemTitle(handle, title.c_str());
	SteamUGC()->SetItemDescription(handle, description.c_str());

	SteamUGC()->SetItemContent(handle, folder.c_str());

	SteamUGC()->SetItemPreview(handle, previewPath.c_str());

	for (std::string s : previewPictures)
	{
		SteamUGC()->AddItemPreviewFile(handle, s.c_str(), EItemPreviewType::k_EItemPreviewType_Image);
	}

	SteamAPICall_t hSteamAPICall = SteamUGC()->SubmitItemUpdate(handle, "Initial upload");
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