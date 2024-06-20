/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "FOnline.h"
#include "../../Multiplayer/Packets/CPacketServerList.h"
#include <Themida/ThemidaSDK.h>

float lastSend = 0;

bool CanSendRequest(float spam = 0.5f)
{
	VM_START
	if (lastSend + spam < time(NULL))
	{
		lastSend = time(NULL);
		VM_END
		return true;
	}

	VM_END
	return false;
}

void Average4k::Api::Functions::FOnline::QueryLobbies()
{
	if (!CanSendRequest(2))
		return;

	Multiplayer::Packets::CPacketServerList packet{};

	Multiplayer::Connection::Instance->sendMessage(packet);
}

sol::table Average4k::Api::Functions::FOnline::GetLobbies()
{

	return sol::table();
}
