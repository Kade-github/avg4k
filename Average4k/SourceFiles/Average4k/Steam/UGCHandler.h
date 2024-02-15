/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef AVG4K_UGCHANDLER_H
#define AVG4K_UGCHANDLER_H

#include <steam/steam_api.h>
#include <map>
#include <vector>
#include <string>
namespace Average4k::Steam
{
	class UGCHandler
	{
	public:
		static UGCHandler* Instance;

		UGCHandler()
		{
			Instance = this;
		}

		PublishedFileId_t currentItem;

		UGCUpdateHandle_t currentItem_updateHandle;

		PublishedFileId_t* subscribedItems;

		UGCQueryHandle_t subscribedItems_queryHandle;

		std::map<std::string, std::string> subscribedPacks;
		std::map<std::string, std::string> subscribedNoteskins;
		std::map<std::string, std::string> subscribedThemes;

		bool isDone = false;
		bool findingSubscribedItems = false;

		float GetCurrentItemProgress();

		void onItemCreated(CreateItemResult_t* pCallback, bool bIOFailure);
		void onItemSubmitted(SubmitItemUpdateResult_t* pCallback, bool bIOFailure);
		void onItemDeleted(DeleteItemResult_t* pCallback, bool bIOFailure);
		void onSubscribedItems(SteamUGCQueryCompleted_t* pCallback, bool bIOFailure);

		void UploadPack(std::string folder, std::string previewPath, std::string title, std::string description, std::vector<std::string> tags);
		void UploadNoteskin(std::string folder, std::string previewPath, std::string title, std::string description, std::vector<std::string> previewPictures, std::vector<std::string> tags);
		void UploadTheme(std::string folder, std::string previewPath, std::string title, std::string description, std::vector<std::string> previewPictures, std::vector<std::string> tags);

		void PopulateSubscribedItems();

		CCallResult<UGCHandler, CreateItemResult_t> m_CreateItemResult;
		CCallResult<UGCHandler, SubmitItemUpdateResult_t> m_SubmitItemUpdateResult;
		CCallResult<UGCHandler, DeleteItemResult_t> m_DeleteItemResult;
		CCallResult<UGCHandler, SteamUGCQueryCompleted_t> m_SubscribedItemsResult;

		void CreateItem();
		void DeleteItem();
	};
}

#endif