#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "includes.h"
class Steam
{
public:
	Steam() {};
	void InitSteam();

	void ShutdownSteam();

	SDL_Texture* getAvatar(std::string url);
};

