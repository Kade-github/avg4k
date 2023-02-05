#include "SoundManager.h"
std::map<std::string, Channel*> SoundManager::channels = {};
bool SoundManager::isThreadDone = true;
Channel* SoundManager::threadLoaded = NULL;
std::string SoundManager::threadPath = "";

static void CALLBACK SyncProc(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	Channel* ch = ((Channel*)user);
	ch->isPlaying = false;
	if (ch->dieAfterPlay)
	{
		SoundManager::removeChannel(ch->name);
		return;
	}
	if (ch->loop)
		ch->play();
}

Channel::Channel(unsigned long channelId)
{
	id = channelId;

	setLength();

	BASS_ChannelSetSync(id, BASS_SYNC_END, 0, SyncProc, this);
}