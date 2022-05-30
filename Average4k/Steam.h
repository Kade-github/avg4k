#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "QuaverFile.h"
#include "SMFile.h"
#include "stbi.h"
#include <map>
#include "Pack.h"


struct steamItem {
	SteamUGCDetails_t details;
	std::string chartType;
	std::string chartFile;
	bool isPackFolder;
	char folder[512];
};

class Steam
{
public:


	static std::map<std::string, Texture*> pixelsForAvatar;

	Steam() {};
	void InitSteam();

	void ShutdownSteam();

	std::string searchName;

	Chart downloadedChart;
	Pack downloadedPack;

	PublishedFileId_t createdId;

	UGCUpdateHandle_t updatehandle;

	static uint64_t downloadId;

	std::vector<steamItem> downloadedList;
	std::vector<steamItem> subscribedList;
	static Texture* getAvatar(const char* url);

	void doesWorkshopItemExist(std::string name);

	void createWorkshopItem();
	void uploadToItem(Chart* c, PublishedFileId_t id, std::string fileName);
	void uploadPack(Pack* p, PublishedFileId_t id);
	void populateWorkshopItems(int page);
	void populateSubscribedItems();
	float CheckWorkshopProgress();
	static float CheckWorkshopDownload();
	static void LoadWorkshopChart(uint64_t publishedFileId);
	char chartWorkshop[512];
	static std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace);
private:
	STEAM_CALLBACK(Steam, CallbackDownload, DownloadItemResult_t);
	CCallResult<Steam, SteamUGCQueryCompleted_t> UGCQueryCallback;
	void OnUGCQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure);
	CCallResult<Steam, CreateItemResult_t> CreateItemCallback;
	void OnCreateItemCallback(CreateItemResult_t* result, bool bIOFailure);
	CCallResult<Steam, SteamUGCQueryCompleted_t> Name;
	void OnName(SteamUGCQueryCompleted_t* result, bool bIOFailure);
	CCallResult<Steam, SubmitItemUpdateResult_t> UploadedItemCallback;

	void OnUploadedItemCallback(SubmitItemUpdateResult_t* result, bool bIOFailure);
	CCallResult<Steam, SteamUGCQueryCompleted_t> UGCAllQueryCallback;
	void OnUGCAllQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure);
	CCallResult<Steam, SteamUGCQueryCompleted_t> UGCSubscribedQueryCallback;
	void OnUGCSubscribedQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure);
};

