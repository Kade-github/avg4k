#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "includes.h"
#include "Chart.h"
#include "QuaverFile.h"
#include "SMFile.h"
#include <map>

struct steamItem {
	SteamUGCDetails_t details;
	std::string chartType;
	std::string chartFile;
	char folder[512];
};

class Steam
{
public:


	Steam() {};
	void InitSteam();

	void ShutdownSteam();

	Chart* downloadedChart;

	std::vector<steamItem> downloadedList;
	std::vector<steamItem> subscribedList;
	static SDL_Texture* getAvatar(const char* url);

	void populateWorkshopItems(int page);
	void populateSubscribedItems();

	static void LoadWorkshopChart(unsigned long publishedFileId);
	char chartWorkshop[512];
	static std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace);
private:
	STEAM_CALLBACK(Steam, CallbackDownload, DownloadItemResult_t);
	CCallResult<Steam, SteamUGCQueryCompleted_t> UGCQueryCallback;
	void OnUGCQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure);
	CCallResult<Steam, SteamUGCQueryCompleted_t> UGCAllQueryCallback;
	void OnUGCAllQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure);
	CCallResult<Steam, SteamUGCQueryCompleted_t> UGCSubscribedQueryCallback;
	void OnUGCSubscribedQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure);
};

