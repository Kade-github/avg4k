#include "Steam.h"

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
