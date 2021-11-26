#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "includes.h"
#include <map>
class Steam
{
public:
	Steam() {};
	void InitSteam();

	void ShutdownSteam();

	static SDL_Texture* getAvatar(const char* url);

	static void LoadWorkshopChart(unsigned long publishedFileId);
	
private:
	STEAM_CALLBACK(Steam, CallbackDownload, DownloadItemResult_t);
	CCallResult<Steam, SteamUGCQueryCompleted_t> UGCQueryCallback;
	void OnUGCQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure);

	char chartWorkshop[512];
};

