#include "Steam.h"
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <SDL_image.h>
#include "Game.h"
#include <regex>

void Steam::InitSteam()
{
	if (!SteamAPI_Init())
	{
		std::cout << "uh oh, you aren't on steam (or you don't own the game)" << std::endl;
	}
}

void Steam::ShutdownSteam()
{
	SteamAPI_Shutdown();
}

extern "C"
{
    struct MemoryStruct {
        char* memory;
        size_t size;
    };

    static size_t
        WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        size_t realsize = size * nmemb;
        struct MemoryStruct* mem = (struct MemoryStruct*)userp;

        char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
        if (!ptr) {
            /* out of memory! */
            printf("not enough memory (realloc returned NULL)\n");
            return 0;
        }

        mem->memory = ptr;
        memcpy(&(mem->memory[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;

        return realsize;
    }
}


SDL_Texture* Steam::getAvatar(const char* url)
{
	CURL* curlCtx = curl_easy_init();

    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1); 
    chunk.size = 0; 

    std::cout << "loading " << url << std::endl;

    curl_easy_setopt(curlCtx, CURLOPT_URL, url);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curlCtx, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.94 Safari/537.36");

    std::cout << "downloading avatar" << std::endl;

    CURLcode rc = curl_easy_perform(curlCtx);
    if (rc != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(rc));

    std::cout << "downloaded! " << chunk.size << std::endl;

    SDL_RWops* rw = SDL_RWFromConstMem(chunk.memory, chunk.size);

    curl_easy_cleanup(curlCtx);

    std::cout << "trying to texture bruh" << std::endl;

    SDL_Texture* tex = IMG_LoadTexture_RW(Game::renderer, rw, true);

    std::cout << "textured " << std::endl;

    if (!tex)
        return NULL;

    free(chunk.memory);

    return tex;
}


// show me the money

void Steam::populateWorkshopItems(int page) {
    UGCQueryHandle_t h = SteamUGC()->CreateQueryAllUGCRequest(k_EUGCQuery_RankedByVote, k_EUGCMatchingUGCType_Items_ReadyToUse, 1828580, 1828580, page);
    
    SteamUGC()->SetReturnKeyValueTags(h, true);

    auto handle = SteamUGC()->SendQueryUGCRequest(h);

    UGCQueryCallback.Set(handle, this, &Steam::OnUGCAllQueryCallback);

    SteamUGC()->ReleaseQueryUGCRequest(h);
}

void Steam::OnUGCAllQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure)
{
    downloadedList.clear();
    for (int i = 0; i < result->m_unNumResultsReturned; i++)
    {
        SteamUGCDetails_t id;
        bool yes = SteamUGC()->GetQueryUGCResult(result->m_handle, i, &id);
        if (yes)
        {
            char* chartType = (char*)malloc(512);
            char* chartFile = (char*)malloc(512);
            SteamUGC()->GetQueryUGCKeyValueTag(result->m_handle, i, "chartType", chartType, 512);
            SteamUGC()->GetQueryUGCKeyValueTag(result->m_handle, i, "chartFile", chartFile, 512);

            std::string chartTypestd(chartType);
            std::string chartFilestd(chartFile);


            steamItem i;
            i.details = id;
            i.chartType = std::string(chartType);
            i.chartFile = std::string(chartFile);
            uint64 sizeOnDisk;
            uint32 timestamp;
            SteamUGC()->GetItemInstallInfo(id.m_nPublishedFileId, &sizeOnDisk, i.folder, sizeof(i.folder), &timestamp);

            subscribedList.push_back(i);
        }
    }
}

// get subscribe

void Steam::populateSubscribedItems()
{
    const int ahg = SteamUGC()->GetNumSubscribedItems();

    PublishedFileId_t* subscribed = (PublishedFileId_t*)malloc(sizeof(PublishedFileId_t) * ahg);

    SteamUGC()->GetSubscribedItems(subscribed, ahg);

    UGCQueryHandle_t h = SteamUGC()->CreateQueryUGCDetailsRequest(subscribed,ahg);

    SteamUGC()->SetReturnKeyValueTags(h, true);

    auto handle = SteamUGC()->SendQueryUGCRequest(h);

    UGCQueryCallback.Set(handle, this, &Steam::OnUGCSubscribedQueryCallback);

    SteamUGC()->ReleaseQueryUGCRequest(h);

    free(subscribed);
}

void Steam::OnUGCSubscribedQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure)
{
    subscribedList.clear();
    for (int i = 0; i < result->m_unNumResultsReturned; i++)
    {
        SteamUGCDetails_t id;
        bool yes = SteamUGC()->GetQueryUGCResult(result->m_handle, i, &id);
        if (yes)
        {
            char* chartType = (char*)malloc(512);
            char* chartFile = (char*)malloc(512);
            SteamUGC()->GetQueryUGCKeyValueTag(result->m_handle, i, "chartType", chartType, 512);
            SteamUGC()->GetQueryUGCKeyValueTag(result->m_handle, i, "chartFile", chartFile, 512);


            steamItem i;
            i.details = id;
            i.chartType = std::string(chartType);
            i.chartFile = std::string(chartFile);
            uint64 sizeOnDisk;
            uint32 timestamp;
            SteamUGC()->GetItemInstallInfo(id.m_nPublishedFileId, &sizeOnDisk, i.folder, sizeof(i.folder), &timestamp);
            subscribedList.push_back(i);
        }
        else
        {
            std::cout << "failed to fuck wit item #" << i << std::endl;
        }
    }
}


// loading and downloading I got orkshop charts

void Steam::LoadWorkshopChart(uint64_t publishedFileID) {

    PublishedFileId_t file = publishedFileID;

    std::cout << "downloading " << file << std::endl;

    bool success = SteamUGC()->DownloadItem(file, true);

    std::cout << "Started steam download: " << success << std::endl;
    
}

void Steam::CallbackDownload(DownloadItemResult_t* res) {

    if (res->m_unAppID != 1828580)
        return;

    if (res->m_eResult != k_EResultOK) {
        std::cout << "Steam download callback error: " << res->m_eResult << std::endl;
        return;
   }
 
    uint64 sizeOnDisk;
    char directory[512];
    uint32 timestamp;

    bool infoSuccess = SteamUGC()->GetItemInstallInfo(res->m_nPublishedFileId, &sizeOnDisk, this->chartWorkshop, sizeof(this->chartWorkshop), &timestamp);

    if (!infoSuccess) {
        std::cout << "Error while getting item install information." << std::endl;
        return;
    }

    std::cout << "Chart is " << sizeOnDisk << " bytes" << std::endl;

    UGCQueryHandle_t req = SteamUGC()->CreateQueryUGCDetailsRequest(&res->m_nPublishedFileId, 1);

    SteamUGC()->SetReturnKeyValueTags(req, true);

    auto handle = SteamUGC()->SendQueryUGCRequest(req);

    UGCQueryCallback.Set(handle, this, &Steam::OnUGCQueryCallback);

    SteamUGC()->ReleaseQueryUGCRequest(req);
}
//haha stackoverflow go brrr (shut the fuck up i can hear you laughing)
// dw I do it too :)
std::string Steam::ReplaceString(std::string subject, const std::string& search,
    const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}

void Steam::OnUGCQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure)
{
    if (result->m_unNumResultsReturned != 1) {
        std::cout << "We got something other than 1 result, this shouldn't happen" << std::endl;
        return;
    }

    char* chartType = (char*)malloc(512);
    char* chartFile = (char*)malloc(512);
    SteamUGC()->GetQueryUGCKeyValueTag(result->m_handle, 0, "chartType", chartType, 512);
    SteamUGC()->GetQueryUGCKeyValueTag(result->m_handle, 0, "chartFile", chartFile, 512);

    
    chartMeta meta;
    bool parsed{};


    std::string chartTypestd(chartType);
    std::string chartFilestd(chartFile);
    std::string path(this->chartWorkshop);

    if (chartTypestd == "qv") {
        QuaverFile* file = new QuaverFile();
        meta = file->returnChart(path);
        delete file;
        parsed = true;
    }
    if (chartTypestd == "sm") {

        std::string fullPath = path + "\\" + chartFilestd;

        std::string replaced = ReplaceString(fullPath, "\\", "/");
        std::string replaced2 = ReplaceString(path, "\\", "/");
     
        SMFile* smFile = new SMFile(replaced, replaced2, false);
        meta = smFile->meta;
        delete smFile;
        parsed = true;
    }

    if (parsed) {
        Steam::downloadedChart = new Chart(meta);

        CPacketClientChartAcquired acquired;
        acquired.PacketType = eCPacketClientChartAcquired;
        acquired.Order = 0;

        Multiplayer::sendMessage<CPacketClientChartAcquired>(acquired);

    }

}