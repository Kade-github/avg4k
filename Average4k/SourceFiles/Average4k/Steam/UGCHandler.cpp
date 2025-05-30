#include "UGCHandler.h"

#include <AvgEngine/Utils/Logging.h>
#include <AvgEngine/Utils/StringTools.h>

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
		return prog;
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

	SteamUGC()->SubscribeItem(pCallback->m_nPublishedFileId);

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

void Average4k::Steam::UGCHandler::onSubscribedItems(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure)
{
	if (bIOFailure)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Failed to query subscribed items. Error: " + std::to_string(pCallback->m_eResult));
		return;
	}

	AvgEngine::Logging::writeLog("[Steam] Subscribed items query completed.");

	uint32_t count = pCallback->m_unNumResultsReturned;

	subscribedPacks.clear();
	subscribedNoteskins.clear();
	subscribedThemes.clear();

	for (int i = 0; i < count; i++)
	{
		SteamUGCDetails_t details;
		bool y = SteamUGC()->GetQueryUGCResult(subscribedItems_queryHandle, i, &details);

		if (!y)
			continue;

		if (details.m_eVisibility == k_ERemoteStoragePublishedFileVisibilityPrivate)
			continue;

		std::string s = details.m_rgchTags;
		std::string title = details.m_rgchTitle;
		char folder[MAX_PATH] = "";
		uint64 sizeOnDisk;
		uint32 timestamp;

		SteamUGC()->GetItemInstallInfo(details.m_nPublishedFileId, &sizeOnDisk, folder, sizeof(folder), &timestamp);

		if (s.contains("Packs"))
			subscribedPacks[title] = folder;
		else if (s.contains("Noteskins"))
			subscribedNoteskins[title] = folder;
		else if (s.contains("Themes"))
			subscribedThemes[title] = folder;

		AvgEngine::Logging::writeLog("[Steam] [Subscriptions] Found item: " + title);


	}


	SteamUGC()->ReleaseQueryUGCRequest(subscribedItems_queryHandle);
	subscribedItems_queryHandle = 0;

	findingSubscribedItems = false;
}

void Average4k::Steam::UGCHandler::UploadPack(std::string folder, std::string previewPath, std::string title, std::string description, std::vector<std::string> tags)
{
	if (tags.size() == 0)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] No tags provided for pack.");
		return;
	}

	// check if it has any tags that aren't correct (like, themes, noteskins, downscroll, upscroll, etc)

	if (std::find(tags.begin(), tags.end(), "Themes") != tags.end() || 
		std::find(tags.begin(), tags.end(), "Noteskins") != tags.end() || 
		std::find(tags.begin(), tags.end(), "Downscroll") != tags.end() ||
		std::find(tags.begin(), tags.end(), "Upscroll") != tags.end()) {
		AvgEngine::Logging::writeLog("[Steam] [Error] Invalid tags provided for pack.");
		return;
	}

	// check if it has the right tag

	if (std::find(tags.begin(), tags.end(), "Packs") == tags.end())
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Invalid tags provided for pack.");
		return;
	}

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
	if (tags.size() == 0)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] No tags provided for noteskin.");
		return;
	}

	// check if it has any tags that aren't correct (like, themes, pack, beginner, easy, medium, hard, challenge, edit, etc)

	if (std::find(tags.begin(), tags.end(), "Themes") != tags.end() ||
		std::find(tags.begin(), tags.end(), "Packs") != tags.end() ||
		std::find(tags.begin(), tags.end(), "Beginner") != tags.end() ||
		std::find(tags.begin(), tags.end(), "Easy") != tags.end() ||
		std::find(tags.begin(), tags.end(), "Medium") != tags.end() ||
		std::find(tags.begin(), tags.end(), "Hard") != tags.end() ||
		std::find(tags.begin(), tags.end(), "Challenge") != tags.end() ||
		std::find(tags.begin(), tags.end(), "Edit") != tags.end()) {
		AvgEngine::Logging::writeLog("[Steam] [Error] Invalid tags provided for noteskin.");
		return;
	}

	// check if it has the right tag

	if (std::find(tags.begin(), tags.end(), "Noteskin") == tags.end())
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Invalid tags provided for noteskin.");
		return;
	}

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
	if (tags.size() == 0)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] No tags provided for theme.");
		return;
	}

	// check if it has any tags that aren't correct

	if (tags.size() > 1)
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Invalid tags provided for theme.");
		return;
	}

	if (tags[0] != "Themes")
	{
		AvgEngine::Logging::writeLog("[Steam] [Error] Invalid tags provided for theme.");
		return;
	}


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

void Average4k::Steam::UGCHandler::PopulateSubscribedItems()
{
	if (subscribedItems_queryHandle != 0)
	{
		AvgEngine::Logging::writeLog("[Steam] [Warning] Subscribed items query already in progress.");
		return;
	}

	if (subscribedItems != nullptr)
	{
		delete[] subscribedItems;
	}

	uint32_t count = SteamUGC()->GetNumSubscribedItems();
	subscribedItems = new PublishedFileId_t[count];
	SteamUGC()->GetSubscribedItems(subscribedItems, count);

	AvgEngine::Logging::writeLog("[Steam] Subscribed items count: " + std::to_string(count));

	subscribedItems_queryHandle = SteamUGC()->CreateQueryUGCDetailsRequest(subscribedItems, count);

	SteamAPICall_t hSteamAPICall = SteamUGC()->SendQueryUGCRequest(subscribedItems_queryHandle);
	m_SubscribedItemsResult.Set(hSteamAPICall, this, &UGCHandler::onSubscribedItems);

	findingSubscribedItems = true;
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