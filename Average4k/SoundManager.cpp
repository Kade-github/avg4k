#include "SoundManager.h"
std::map<std::string, Channel*> SoundManager::channels = {};
bool SoundManager::isThreadDone = true;
Channel* SoundManager::threadLoaded = NULL;
std::string SoundManager::threadPath = "";