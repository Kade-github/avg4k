#pragma once
#include "includes.h"
#include "bass_fx.h"
#include "MiniBpm.h"


class Channel {
public:
	float bpm = 0; // this is used to set the bpm, when ever we want to.
	std::vector<double> bpmCan;
	unsigned long id = -1;
	unsigned long decodeChan = -1;
	float rate = 1;
	int length = 0;

	float detectedBPMProg = 0;

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
		if (decodeChan != -1)
			BASS_ChannelFree(decodeChan);
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
		return (BASS_ChannelBytes2Seconds(id, BASS_ChannelGetPosition(id, BASS_POS_BYTE))) * 1000;
	}

	float returnSampleRate()
	{
		float sample;
		BASS_ChannelGetAttribute(id, BASS_ATTRIB_FREQ, &sample);
		return sample;
	}

	void bpmDetect(float* samples, int length, bool freeSamples = false)
	{

		breakfastquay::MiniBPM finder = breakfastquay::MiniBPM(returnSampleRate());

		finder.setBPMRange(55, 190);
		float banger = (float)finder.estimateTempoOfSamples(samples, length);
		bpmCan = finder.getTempoCandidates();
		if (freeSamples)
			std::free(samples);

		bpm = banger;
	}

	float* returnSongSample(float* sampleLength)
	{
		if (decodeChan == -1)
		{
			decodeChan = BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE);
		}

		float leng = BASS_ChannelGetLength(decodeChan, BASS_POS_BYTE);

		float* samples = (float*)std::malloc(leng);

		leng = BASS_ChannelGetData(decodeChan, samples, leng);
		*sampleLength = leng;

		if (BASS_ErrorGetCode() != 0)
			std::cout << "bass error " << BASS_ErrorGetCode() << std::endl;

		BASS_ChannelSetPosition(decodeChan, BASS_ChannelSeconds2Bytes(decodeChan, 0), NULL);

		return samples;
	}

	float* returnSamples(float length, float* nonFFTLength, bool FFT = true)
	{
		// FREE THIS KADE :))))
		float* samples = (float*)std::malloc(sizeof(float) * length);

		if (decodeChan == -1)
		{
			decodeChan = BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE);
		}

		if (FFT)
			BASS_ChannelGetData(decodeChan, samples, BASS_DATA_FFT_COMPLEX);

		std::cout << BASS_ErrorGetCode() << std::endl;

		return samples;
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
};

class SoundManager
{
public:
	static std::map<std::string, Channel*> channels;

	static Channel* getChannelByName(std::string name)
	{
		return channels[name];
	}

	static Channel* getChannelById(unsigned long id, bool testBPMChannel = false)
	{
		Channel* notFound = nullptr;
		for (std::map<std::string, Channel*>::iterator iter = channels.begin(); iter != channels.end(); ++iter)
		{
			if (iter->second->id == id)
				return iter->second;
		}
		return notFound;
	}

	static void removeChannel(std::string name)
	{
		channels.erase(channels.find(name));
	}

	static Channel* createChannel(std::string path, std::string name, bool autoFree = false)
	{
		if (channels[name] != NULL)
			delete channels[name];
		if (autoFree)
			channels[name] = new Channel(BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT | BASS_STREAM_AUTOFREE));
		else
			channels[name] = new Channel(BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT));
		channels[name]->path = path;
		channels[name]->setVolume(0.2);
		return channels[name];
	}
};
