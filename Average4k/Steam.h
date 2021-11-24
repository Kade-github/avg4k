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

	static std::map<std::string, SDL_Texture*>* cachedAvatars;

	static SDL_Texture* getAvatar(const char* url);
};

