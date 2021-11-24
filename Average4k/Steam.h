#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "includes.h"
class Steam
{
public:
	Steam() {};
	void InitSteam();

	void ShutdownSteam();

	static SDL_Texture* getAvatar(const char* url);
};

