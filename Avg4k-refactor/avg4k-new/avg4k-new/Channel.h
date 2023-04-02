#pragma once
#include "includes.h"
#include "bass_fx.h"
namespace AvgEngine::Audio
{
	class Channel
	{
	public:
		unsigned long id = -1;
		unsigned long decode = -1;
		char* data;
		bool isPlaying;
		std::string name;
		std::string path;

		float rate = 1;

		float volume = 1;

		int length = 0;

		Channel(unsigned long _id)
		{
			id = _id;
		}

		~Channel()
		{
			if (id == -1)
				return;
			Free();
		}

		/// <summary>
		/// Free up the channel
		/// </summary>
		void Free()
		{
			if (id == -1)
				return;
			BASS_ChannelFree(id);
			id = -1;

			if (data) 
				std::free(data);
		}

		/// <summary>
		/// Set the channel in a playing state
		/// </summary>
		void Play()
		{
			if (id == -1)
				return;
			BASS_ChannelPlay(id, false);
			isPlaying = true;
		}

		/// <summary>
		/// Stop the channel
		/// </summary>
		void Stop()
		{
			if (id == -1)
				return;
			BASS_ChannelStop(id);
		}

		/// <summary>
		/// Get the current position of the channel in seconds
		/// </summary>
		/// <returns>Seconds</returns>
		float GetPos()
		{
			if (id == -1)
				return 0;
			return (BASS_ChannelBytes2Seconds(id, BASS_ChannelGetPosition(id, BASS_POS_BYTE)));
		}

		/// <summary>
		/// Set the current position of the channel
		/// </summary>
		/// <param name="s">Seconds</param>
		void SetPos(float s)
		{
			if (id == -1)
				return;
			auto bytes = BASS_ChannelSeconds2Bytes(id, s);
			BASS_ChannelSetPosition(id, bytes, BASS_POS_BYTE);
		}

		/// <summary>
		/// Get a float* repersentation of the channel
		/// </summary>
		/// <param name="sampleLength">The length</param>
		/// <returns>The sample</returns>
		float* ReturnSongSample(float* sampleLength)
		{
			if (decode == -1)
				decode = BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE);

			float leng = BASS_ChannelGetLength(decode, BASS_POS_BYTE);

			float* samples = (float*)std::malloc(leng);

			leng = BASS_ChannelGetData(decode, samples, leng);
			*sampleLength = leng;

			if (BASS_ErrorGetCode() != 0) {
				Logging::writeLog("[BASS] [Error] Error " + std::to_string(BASS_ErrorGetCode()));
			}
			BASS_ChannelSetPosition(decode, BASS_ChannelSeconds2Bytes(decode, 0), NULL);

			return samples;
		}

		/// <summary>
		/// Get a float* repersentation of the channel (optionally in FTT)
		/// </summary>
		/// <param name="length">The length of the channel</param>
		/// <param name="nonFFTLength">The non FTT lenght of the channel</param>
		/// <param name="FFT">Return an FTT repersentation?</param>
		/// <returns>The samples</returns>
		/// 
		float* ReturnSamples(float length, float* nonFFTLength, bool FFT = true)
		{
			// FREE THIS KADE :))))
			float* samples = (float*)std::malloc(sizeof(float) * length);

			if (decode == -1)
				decode = BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE);

			if (FFT)
				BASS_ChannelGetData(decode, samples, BASS_DATA_FFT_COMPLEX);

			if (BASS_ErrorGetCode() != 0) {
				Logging::writeLog("[BASS] [Error] Error " + std::to_string(BASS_ErrorGetCode()));
			}

			return samples;
		}

		/// <summary>
		/// Get the current channels sample rate
		/// </summary>
		/// <returns>The sample rate</returns>
		float SampleRate()
		{
			float sample;
			BASS_ChannelGetAttribute(id, BASS_ATTRIB_FREQ, &sample);
			return sample;
		}

		/// <summary>
		/// Set the current channels time rate
		/// </summary>
		/// <param name="_rate">The rate to be set</param>
		void RateChange(float _rate)
		{
			if (id == -1)
				return;
			rate = _rate;
			float bassRate = (rate * 100) - 100;
			BASS_ChannelSetAttribute(id, BASS_ATTRIB_TEMPO, bassRate);
		}

		/// <summary>
		/// Convert the channel to a Effects Channel
		/// </summary>
		void ConvertToFX()
		{
			if (id == -1)
				return;
			BASS_ChannelFree(id);
			id = BASS_FX_TempoCreate(BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_STREAM_DECODE), BASS_FX_FREESOURCE);
		}

		/// <summary>
		/// Set the current volume of the current channel
		/// </summary>
		/// <param name="vol">The volume to set</param>
		void SetVolume(float vol)
		{
			if (id == -1)
				return;
			volume = vol;
			BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, vol);
		}

		bool operator==(const Channel& other) {
			return (name == other.name) || (id == other.id);
		}
	};
}