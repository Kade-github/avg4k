#include "Steam.h"

#include "Game.h"
#include "Pack.h"
#include "CPacketJoinServer.h"
#include "Helpers.h"

bool Steam::eulaAccepted = false;

void Steam::InitSteam()
{
    MUTATE_START
        SteamAPI_Init();
    MUTATE_END
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
        MUTATE_START
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
        MUTATE_END
    }
}


Texture* Steam::getAvatar(const char* base64Encode)
{
    MUTATE_START

    size_t outLength;

    char* img = Helpers::base64_decode(base64Encode, std::string(base64Encode).length(), &outLength);

    Texture* tex = stbi_h::stbi_load_memory(img, outLength);

    if (!tex)
        return NULL;
    free(img);

    Game::currentMenu->onSteam("profileDownloaded");

    MUTATE_END
    return tex;
}

void Steam::doesWorkshopItemExist(std::string name)
{
    UGCQueryHandle_t h = SteamUGC()->CreateQueryAllUGCRequest(k_EUGCQuery_RankedByVote, k_EUGCMatchingUGCType_Items_ReadyToUse, 1828580, 0);

    searchName = name;

    SteamUGC()->SetSearchText(h, name.c_str());

    auto handle = SteamUGC()->SendQueryUGCRequest(h);

    Name.Set(handle, this, &Steam::OnName);

    SteamUGC()->ReleaseQueryUGCRequest(h);
}

void Steam::SetPresence(const char* presence)
{
    SteamFriends()->ClearRichPresence();
    SteamFriends()->SetRichPresence("gamestatus", presence);
    SteamFriends()->SetRichPresence("steam_display", "#Status");
}

void Steam::getEula()
{
    SteamAPICall_t call = SteamUGC()->GetWorkshopEULAStatus();
    WorkshopEULACallback.Set(call, this, &Steam::OnWorkshopEULACallback);
}

void Steam::showEula()
{
    if (!SteamUGC()->ShowWorkshopEULA())
    {
        Logging::writeLog("User needs to accept the steam workshop eula, but I can't show it!");
        Game::instance->showErrorWindow("Steam Error", "You need to accept the workshop eula!", false);
    }
}

void Steam::OnName(SteamUGCQueryCompleted_t* result, bool bIOFailure)
{
    MUTATE_START
    for (int i = 0; i < result->m_unNumResultsReturned; i++)
    {
        SteamUGCDetails_t id;
        bool yes = SteamUGC()->GetQueryUGCResult(result->m_handle, i, &id);
        if (yes || id.m_rgchTitle != "")
        {
            if (id.m_rgchTitle == searchName)
            {
                if (Game::currentMenu != nullptr)
                    Game::currentMenu->onSteam("resultExists");
                return;
            }
        }
    }
    Game::currentMenu->onSteam("resultDoesntExist");
    MUTATE_END
}


void Steam::createWorkshopItem()
{
    SteamAPICall_t call = SteamUGC()->CreateItem(1828580, k_EWorkshopFileTypeCommunity);
    CreateItemCallback.Set(call, this, &Steam::OnCreateItemCallback);
}

long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}



void Steam::uploadToItem(Chart* c, PublishedFileId_t id, std::string fileName)
{
    MUTATE_START

    std::vector<std::string> b = Chart::split(c->meta.folder, '/');


    auto handle = SteamUGC()->StartItemUpdate(1828580, id);
    updatehandle = handle;
    SteamUGC()->SetItemTitle(handle, ("[CHART] " + c->meta.songName).c_str());
    std::string desc = "Uploaded from in game.\nDiffs:\n";
    for (difficulty& diff : c->meta.difficulties)
    {
        if (diff.charter.size() != 0)
            desc += diff.name + " - Charted by " + diff.charter + "\n";
        else
            desc += diff.name + " - Charted by a unknown charter\n";
    }
    SteamUGC()->SetItemDescription(handle, desc.c_str());
    SteamUGC()->SetItemUpdateLanguage(handle, "english");
    SteamUGC()->SetItemMetadata(handle, b[b.size() - 2].c_str());
    SteamUGC()->SetItemVisibility(handle, k_ERemoteStoragePublishedFileVisibilityPublic);
    SteamParamStringArray_t* p = (SteamParamStringArray_t*)malloc(sizeof(SteamParamStringArray_t));
    p->m_nNumStrings = 1;
    p->m_ppStrings = const_cast<const char**>((char**)malloc(sizeof(char*)));
    
    char* bruh = (char*)malloc(7 * sizeof(char*));

    strcpy_s(bruh, 7, "Charts");

    p->m_ppStrings[0] = bruh;

    SteamUGC()->SetItemTags(handle, p);
    SteamUGC()->AddItemKeyValueTag(handle, "chartType", c->meta.chartType == 0 ? "sm" : "qv");
    SteamUGC()->AddItemKeyValueTag(handle, "chartFile", fileName.c_str());

    SteamUGC()->SetItemContent(handle, (std::filesystem::current_path().string() + "/" + c->meta.folder).c_str());
    SteamUGC()->SetItemPreview(handle, (std::filesystem::current_path().string() + "/" + c->meta.folder + "/" + c->meta.background).c_str());



    long size = GetFileSize(std::filesystem::current_path().string() + "/" + c->meta.folder + "/" + c->meta.background);

    if (size == -1 || size > 1000000)
    {
        SteamUGC()->SetItemPreview(handle, (std::filesystem::current_path().string() + std::string("/assets/skinDefaults/Menu/bg.png")).c_str());
    }

    SteamAPICall_t call = SteamUGC()->SubmitItemUpdate(handle, "Upload");

    UploadedItemCallback.Set(call, this, &Steam::OnUploadedItemCallback);

    free(p);
    MUTATE_END
}

void Steam::uploadPack(Pack* pac, PublishedFileId_t id)
{
    MUTATE_START

    std::vector<std::string> b = Chart::split(pac->metaPath, '/');

    auto handle = SteamUGC()->StartItemUpdate(1828580, id);
    updatehandle = handle;
    SteamUGC()->SetItemTitle(handle, ("[PACK] " + pac->packName).c_str());
    std::string desc = "Uploaded from in game.\nPack Contains:\n";
    for (Song& s : pac->songs)
    {
        if (s.c.meta.artist.size() != 0)
            desc += s.c.meta.songName + " by " + s.c.meta.artist + "\n";
        else
            desc += s.c.meta.songName + " by a unknown artist\n";
    }
    desc += "\nThis is a pack submission; This contains multiple songs and could be very large.";

    SteamUGC()->SetItemDescription(handle, desc.c_str());
    SteamUGC()->SetItemUpdateLanguage(handle, "english");
    SteamUGC()->SetItemMetadata(handle, b[b.size() - 2].c_str());
    SteamUGC()->SetItemVisibility(handle, k_ERemoteStoragePublishedFileVisibilityPublic);
    SteamParamStringArray_t* p = (SteamParamStringArray_t*)malloc(sizeof(SteamParamStringArray_t));
    p->m_nNumStrings = 1;
    p->m_ppStrings = const_cast<const char**>((char**)malloc(sizeof(char*)));

    char* bruh = (char*)malloc(6 * sizeof(char*));

    strcpy_s(bruh, 6, "Packs");

    p->m_ppStrings[0] = bruh;

    SteamUGC()->SetItemTags(handle, p);
    SteamUGC()->AddItemKeyValueTag(handle, "chartType", "pack");
    SteamUGC()->AddItemKeyValueTag(handle, "chartFile", "this is a pack");

    SteamUGC()->SetItemContent(handle, (std::filesystem::current_path().string() + "/" + pac->folder).c_str());
    SteamUGC()->SetItemPreview(handle, (std::filesystem::current_path().string() + "/" + pac->background).c_str());



    long size = GetFileSize(std::filesystem::current_path().string() + "/" + pac->background);

    if (size == -1 || size > 1000000)
    {
        SteamUGC()->SetItemPreview(handle, (std::filesystem::current_path().string() + std::string("/assets/skinDefaults/Menu/bg.png")).c_str());
    }

    SteamAPICall_t call = SteamUGC()->SubmitItemUpdate(handle, "Upload");

    UploadedItemCallback.Set(call, this, &Steam::OnUploadedItemCallback);

    free(p);
    MUTATE_END
}


void Steam::OnCreateItemCallback(CreateItemResult_t* result, bool bIOFailure)
{
    MUTATE_START
    createdId = result->m_nPublishedFileId;


    if (Game::currentMenu != nullptr)
        Game::currentMenu->onSteam("createdItem");
    MUTATE_END
}

void Steam::OnInvite(GameRichPresenceJoinRequested_t* result)
{
    CPacketJoinServer list;
    list.Order = 0;
    list.PacketType = eCPacketJoinServer;
    std::vector<std::string> st = Chart::split(result->m_rgchConnect, ' ');
    std::string sub = st[1];
    list.LobbyID = std::stoul(sub);


    Multiplayer::sendMessage<CPacketJoinServer>(list);
}

void Steam::OnUploadedItemCallback(SubmitItemUpdateResult_t* result, bool bIOFailure)
{
    MUTATE_START

    if (Game::currentMenu != nullptr)
    {
        if (result->m_eResult != k_EResultOK)
        {
            Logging::writeLog("Failed to upload steam item." + std::string((result->m_bUserNeedsToAcceptWorkshopLegalAgreement ? " User hasn't accepted the Eula for the workshop." : "")));
            Game::currentMenu->onSteam("failedItem");
            SteamUGC()->UnsubscribeItem(createdId);
            SteamUGC()->DeleteItem(createdId);
        }
        else
        {
            populateSubscribedItems();
            SteamUGC()->SubscribeItem(createdId);
            Game::currentMenu->onSteam("uploadItem");
        }
    }
    MUTATE_END
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

            downloadedList.push_back(i);
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

    UGCSubscribedQueryCallback.Set(handle, this, &Steam::OnUGCSubscribedQueryCallback);

    SteamUGC()->ReleaseQueryUGCRequest(h);

    free(subscribed);
}

float Steam::CheckWorkshopProgress()
{
    uint64 bytes;
    uint64 totalBytes;
    SteamUGC()->GetItemUpdateProgress(updatehandle, &bytes, &totalBytes);

    if (bytes == 0 || totalBytes == 0) // dont divide by 0 lol
        return 0;

    return (float)bytes / (float)totalBytes;
}

float Steam::CheckWorkshopDownload()
{
    uint64 bytes;
    uint64 totalBytes;

    SteamUGC()->GetItemDownloadInfo(downloadId, &bytes, &totalBytes);

    if (bytes == 0 || totalBytes == 0) // dont divide by 0 lol
        return 0;

    return (float)bytes / (float)totalBytes;
}

void Steam::OnUGCSubscribedQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure)
{
    MUTATE_START
    subscribedList.clear();
    for (int i = 0; i < result->m_unNumResultsReturned; i++)
    {
        SteamUGCDetails_t id;
        bool yes = SteamUGC()->GetQueryUGCResult(result->m_handle, i, &id);
        if (yes)
        {
            char* chartType = (char*)malloc(512);
            char* chartFile = (char*)malloc(512);
            char* tag = (char*)malloc(512);
            SteamUGC()->GetQueryUGCKeyValueTag(result->m_handle, i, "chartType", chartType, 512);
            SteamUGC()->GetQueryUGCKeyValueTag(result->m_handle, i, "chartFile", chartFile, 512);
            SteamUGC()->GetQueryUGCTagDisplayName(result->m_handle, i, 0, tag, 512);

            steamItem i;
            i.details = id;
            i.chartType = std::string(chartType);
            i.chartFile = std::string(chartFile);
            i.isPackFolder = i.chartType == "pack" ? true : false;

            uint64 sizeOnDisk;
            uint32 timestamp;
            SteamUGC()->GetItemInstallInfo(id.m_nPublishedFileId, &sizeOnDisk, i.folder, sizeof(i.folder), &timestamp);
            subscribedList.push_back(i);
        }
    }
    MUTATE_END
}

uint64_t Steam::downloadId = 0;

// loading and downloading I got orkshop charts

void Steam::LoadWorkshopChart(uint64_t publishedFileID) {

    STR_ENCRYPT_START
    downloadId = publishedFileID;
    PublishedFileId_t file = publishedFileID;


    bool success = SteamUGC()->DownloadItem(file, true);
    STR_ENCRYPT_END
}

void Steam::CallbackDownload(DownloadItemResult_t* res) {

    VM_START
    if (res->m_unAppID != 1828580)
        return;

    if (res->m_eResult != k_EResultOK) {
        return;
    }
 
    uint64 sizeOnDisk;
    char directory[512];
    uint32 timestamp;

    bool infoSuccess = SteamUGC()->GetItemInstallInfo(res->m_nPublishedFileId, &sizeOnDisk, this->chartWorkshop, sizeof(this->chartWorkshop), &timestamp);

    if (!infoSuccess) {
        return;
    }

    UGCQueryHandle_t req = SteamUGC()->CreateQueryUGCDetailsRequest(&res->m_nPublishedFileId, 1);

    SteamUGC()->SetReturnKeyValueTags(req, true);

    auto handle = SteamUGC()->SendQueryUGCRequest(req);

    UGCQueryCallback.Set(handle, this, &Steam::OnUGCQueryCallback);

    SteamUGC()->ReleaseQueryUGCRequest(req);
    VM_END
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

void Steam::OnWorkshopEULACallback(WorkshopEULAStatus_t* result, bool bIOFailure)
{
    if (result->m_nAppID != 1828580)
        return;

    eulaAccepted = !result->m_bAccepted;
}

void Steam::OnUGCQueryCallback(SteamUGCQueryCompleted_t* result, bool bIOFailure)
{
    MUTATE_START

    if (result->m_unNumResultsReturned != 1) {
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
    if (chartTypestd == "pack")
    {
        Steam::downloadedPack = SongGather::gatherPack(path, false);
        if (Game::currentMenu != nullptr)
            Game::currentMenu->onSteam("chartAquired");
    }

    if (parsed) {
        Steam::downloadedChart = Chart(meta, false);

        if (Game::currentMenu != nullptr)
            Game::currentMenu->onSteam("chartAquired");

    }
    MUTATE_END
}