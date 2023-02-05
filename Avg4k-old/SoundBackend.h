#pragma once
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"
#include "includes.h"

enum SoundType {
	MP3 = 0,
	FLAC = 1,
	WAV = 2,
	OGG = 3,
	UNSUPPORTED = 4
};

class SoundChannel
{
private:
	// mp3 object
	drmp3 mp3;
public:
	SoundType type;

	uint32 channels;
	uint32 sampleRate;

	SoundChannel(std::string path)
	{
		MUTATE_START
		if (path.ends_with(".mp3"))
		{
			type = MP3;
			if (!drmp3_init_file(&mp3, path.c_str(), NULL)) {
			}

			channels = mp3.channels;
			sampleRate = mp3.sampleRate;
			
		}
		else
		{
			type = UNSUPPORTED;
		}
		MUTATE_END
	}

	void play()
	{
		switch (type)
		{
		case MP3:

			break;
		}
	}
};