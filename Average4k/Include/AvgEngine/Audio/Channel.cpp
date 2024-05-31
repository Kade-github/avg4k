/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#include <AvgEngine/External/Bass/BASS.h>

void CALLBACK Sync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	AvgEngine::Audio::Channel* c = AvgEngine::External::BASS::GetChannel(channel);
	if (c->isPlaying)
	{
		AvgEngine::Logging::writeLog("[Channel] [Info] Sync callback called, repeating song.");
		c->hasEnded = true;
		c->Stop();
		c->Play();
	}
		
}

void AvgEngine::Audio::Channel::Repeat(bool once)
{
	if (autoFree)
	{
		AvgEngine::Logging::writeLog("[Channel] [Warning] Cannot repeat an AutoFree Channel, " + name);
		return;
	}
	if (once)
		BASS_ChannelSetSync(id, BASS_SYNC_END, 0, Sync, 0);
	else
		BASS_ChannelRemoveSync(id, BASS_SYNC_END);
}
