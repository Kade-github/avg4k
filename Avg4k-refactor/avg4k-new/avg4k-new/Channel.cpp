#include "BASS.h"

void CALLBACK Sync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	AvgEngine::Audio::Channel* c = AvgEngine::External::BASS::GetChannel(channel);
	AvgEngine::Logging::writeLog("[Channel] [Info] Sync callback called, repeating song.");
	c->hasEnded = true;
	c->Stop();
	c->Play();
		
}

void AvgEngine::Audio::Channel::Repeat(bool once)
{
	if (autoFree)
	{
		AvgEngine::Logging::writeLog("[Channel] [Warning] Cannot repeat an AutoFree Channel, " + name);
		return;
	}
	if (once)
		BASS_ChannelSetSync(id, BASS_SYNC_END | BASS_SYNC_ONETIME, 0, Sync, 0);
	else
		BASS_ChannelSetSync(id, BASS_SYNC_END, 0, Sync, 0);
}
