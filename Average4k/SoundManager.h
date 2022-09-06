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

	bool isPlaying = false;
	double volume;
	float rate = 1;
	int length = 0;

	float detectedBPMProg = 0;

	char* musicFile;

	bool loop = false;

	bool isFreed = false;

	std::string path;

	static void CALLBACK SyncProc(HSYNC handle, DWORD channel, DWORD data, void* user)
	{
		Channel* ch = ((Channel*)user);
		ch->isPlaying = false;
		if (ch->loop)
			ch->play();
	}

	Channel(unsigned long channelId)
	{
		id = channelId;

		setLength();

		BASS_ChannelSetSync(id, BASS_SYNC_END, 0, SyncProc, this);
	}

	void setLength()
	{
		QWORD word = BASS_ChannelGetLength(id, BASS_POS_BYTE);


		length = BASS_ChannelBytes2Seconds(id, word) * 1000;
	}

	~Channel()
	{
		if (id == -1)
			return;
		free();
		std::cout << "[BASS] deleted " << path << std::endl;
	}

	void free()
	{
		if (id == -1)
			return;
		if (decodeChan != -1)
			BASS_ChannelFree(decodeChan);
		BASS_ChannelFree(id);
		id = -1;

		if (musicFile && !isFreed) {
			isFreed = true;
			std::free(musicFile);
			musicFile = nullptr;
		}
	}

	void play()
	{
		if (id == -1)
			return;
		BASS_ChannelPlay(id, false);
		isPlaying = true;
	}


	void stop()
	{
		if (id == -1)
			return;
		BASS_ChannelStop(id);
	}

	void setVolume(double vol)
	{
		if (id == -1)
			return;
		volume = vol;
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

		if (BASS_ErrorGetCode() != 0) {
			STR_ENCRYPT_START
			std::cout << "bass error " << BASS_ErrorGetCode() << std::endl;
			STR_ENCRYPT_END
		}
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
	static Channel* threadLoaded;
	static std::string threadPath;
	static bool isThreadDone;
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
		
		for (auto pair : channels)
		{
			if (pair.first == name)
				delete pair.second;
		}
		channels.erase(name);
	}

	static void throwShitOntoVector(Channel* ch, std::string name)
	{
		if (channels[name] != NULL)
			delete channels[name];

		channels[name] = ch;
	}

	static void createChannelThread(std::string path, bool autoFree = true)
	{
		if (isThreadDone)
		{
			isThreadDone = false;
			std::thread t([path, autoFree]() {
				//channels[name] = new Channel(BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT | BASS_STREAM_AUTOFREE | BASS_ASYNCFILE));
				auto flags = BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT;

				if (autoFree)
					flags |= BASS_STREAM_AUTOFREE;


				std::ostringstream buf;
				std::ifstream inputFile(path, std::ios::binary | std::ios::in);
				buf << inputFile.rdbuf();
				inputFile.close();

				char* buff = (char*)std::malloc(buf.str().size());

				memcpy(buff, buf.str().data(), buf.str().size());

				auto val = BASS_StreamCreateFile(true, buff, 0, buf.str().size(), flags);

				std::cout << "[BASS] Loading " << path.c_str() << " size: " << buf.str().size() << " by threading." << std::endl;

				if (val == 0) {
					auto error = BASS_ErrorGetCode();

					std::cout << "Bass error! " << error << std::endl;
					std::free(buff);
					return new Channel(-1);
				}

				Channel* ch = new Channel(val);

				ch->musicFile = buff;

				ch->path = path;
				ch->setVolume(0.2);

				ch->setLength();

				threadLoaded = ch;
				isThreadDone = true;
				threadPath = path;

				});
			t.detach();
		}
	}

	static Channel* createChannel(std::string path, std::string name, bool autoFree = false)
	{
		if (channels[name] != NULL)
			delete channels[name];
		//channels[name] = new Channel(BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT | BASS_STREAM_AUTOFREE | BASS_ASYNCFILE));
		auto flags = BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT;

		if (autoFree)
			flags |= BASS_STREAM_AUTOFREE;

		
		std::ostringstream buf;
		std::ifstream inputFile(path, std::ios::binary | std::ios::in);
		buf << inputFile.rdbuf();
		inputFile.close();

		std::cout << "[BASS] Loading " << path.c_str() << " size: " << buf.str().size() << std::endl;

		char* buff = (char*)std::malloc(buf.str().size());
		
		memcpy(buff, buf.str().data(), buf.str().size());

		auto val = BASS_StreamCreateFile(true, buff, 0, buf.str().size(), flags);

		if (val == 0) {
			auto error = BASS_ErrorGetCode();

			std::cout << "Bass error! " << error << std::endl;
			std::free(buff);
			return new Channel(-1);
		}

		channels[name] = new Channel(val);
		
		channels[name]->musicFile = buff;

		channels[name]->path = path;
		channels[name]->setVolume(0.2);

		channels[name]->setLength();

		return channels[name];
	}
};
