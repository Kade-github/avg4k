#pragma once
#include "includes.h"
#include "bass_fx.h"
#include "Chart.h"
class Channel {
public:
	float bpm = 0; // this is used to set the bpm, when ever we want to.
	unsigned long id = -1;
	float rate = 1;
	int length = 0;
	std::string path;
	Channel(unsigned long channelId)
	{
		id = channelId;
		QWORD word = BASS_ChannelGetLength(id, BASS_POS_BYTE);

		length = BASS_ChannelBytes2Seconds(id, word) * 1000;
	}
	~Channel()
	{
		if (id == -1)
			return;
		BASS_ChannelFree(id);
	}

	void free()
	{
		if (id == -1)
			return;
		BASS_ChannelFree(id);
	}

	void play()
	{
		if (id == -1)
			return;
		BASS_ChannelPlay(id, false);
	}


	void stop()
	{
		if (id == -1)
			return;
		BASS_ChannelStop(id);
	}

	void setVolume(float vol)
	{
		if (id == -1)
			return;
		BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, vol);
	}

	float getPos()
	{
		if (id == -1)
			return 0;
		return BASS_ChannelBytes2Seconds(id, BASS_ChannelGetPosition(id, BASS_POS_BYTE)) * 1000;
	}

	void setPos(float ms)
	{
		if (id == -1)
			return;
		auto bytes = BASS_ChannelSeconds2Bytes(id, ms / 1000);
		BASS_ChannelSetPosition(id, bytes, BASS_POS_BYTE);
	}

	void rateChange(float _rate)
	{
		if (id == -1)
			return;
		rate = _rate;
		float bassRate = (rate * 100) - 100;
		BASS_ChannelSetAttribute(id, BASS_ATTRIB_TEMPO, bassRate);
	}

	void createFXStream()
	{
		if (id == -1)
			return;
		BASS_ChannelFree(id);
		id = BASS_FX_TempoCreate(BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_STREAM_DECODE), BASS_FX_FREESOURCE);
	}
	// little helper for this shit
	bpmSegment getBPMSegmentFromChart(Chart* chart)
	{
		if (id == -1)
		{
			bpmSegment seg;
			return seg;
		}
		float pos = getPos();
		bpmSegment seg = chart->getSegmentFromTime(pos);
		return seg;
	}
};

class SoundManager
{
public:
	static std::map<std::string, Channel*> channels;

	static Channel* getChannelByName(std::string name)
	{
		return channels[name];
	}

	static void removeChannel(std::string name)
	{
		channels.erase(channels.find(name));
	}

	static Channel* createChannel(std::string path, std::string name)
	{
		if (channels[name] != NULL)
			delete channels[name];
		channels[name] = new Channel(BASS_StreamCreateFile(false, path.c_str(), 0, 0, 0));
		channels[name]->path = path;
		return channels[name];
	}
};
