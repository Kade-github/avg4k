#include "Steam.h"
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <SDL_image.h>
#include "Game.h"
#include "Chart.h"

std::map<std::string, SDL_Texture*>* Steam::cachedAvatars;

void Steam::InitSteam()
{
    cachedAvatars = new std::map< std::string, SDL_Texture*>();
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
    std::vector<std::string> split = Chart::split(url, '/');
    if (cachedAvatars->contains(split[split.size() - 1]))
        return (*cachedAvatars)[split[split.size() - 1]];

	CURL* curlCtx = curl_easy_init();

    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1); 
    chunk.size = 0; 

    std::cout << "loading " << url << std::endl;

    std::ostringstream stream;
    curl_easy_setopt(curlCtx, CURLOPT_URL, url);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curlCtx, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.94 Safari/537.36");
    long http_code = 0;
    std::cout << "downloading avatar" << std::endl;

    CURLcode rc = curl_easy_perform(curlCtx);
    curl_easy_getinfo(curlCtx, CURLINFO_RESPONSE_CODE, &http_code);
    if (rc != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(rc));

    std::cout << "downloaded! " << http_code << std::endl;

    SDL_RWops* rw = SDL_RWFromMem(chunk.memory, chunk.size);
    curl_easy_cleanup(curlCtx);

    SDL_Surface* surf = IMG_LoadJPG_RW(rw);

    free(chunk.memory);

    std::cout << "freed" << std::endl;

    SDL_RWclose(rw);

    std::cout << "closed" << std::endl;
    
    if (!surf)
        return NULL;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, surf);

    SDL_FreeSurface(surf);

    (*cachedAvatars)[split[split.size() - 1]] = tex;

    std::cout << "surf" << std::endl;

    return tex;
}
