/*
	Copyright 2021-2023 AvgEngine - Kade

	Use of this source code without explict permission from owner is strictly prohibited.
*/

#pragma once
#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>

#include <Bass/bass.h>
#include <Bass/bass_fx.h>

#include <AvgEngine/Utils/Logging.h>

namespace AvgEngine::Audio
{
	class Channel
	{
	public:
		bool hasEnded = false;
		unsigned long id = -1;
		unsigned long decode = -1;
		bool autoFree = false;
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
			data = NULL;
			isPlaying = false;
		}

		~Channel()
		{
			if (id == -1)
				return;
			Free();
		}

		void Repeat(bool once);

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
		void Play(bool restart = true)
		{
			if (id == -1)
				return;
			if (!BASS_ChannelPlay(id, restart))
				Logging::writeLog("[BASS] [Error] Failed to play channel: " + std::to_string(BASS_ErrorGetCode()));
			isPlaying = true;
		}

		/// <summary>
		/// Stop the channel
		/// </summary>
		void Stop()
		{
			if (id == -1)
				return;
			if (!BASS_ChannelPause(id))
				Logging::writeLog("[BASS] [Error] Failed to pause channel: " + std::to_string(BASS_ErrorGetCode()));
			isPlaying = false;
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
			if (!BASS_ChannelSetPosition(id, bytes, BASS_POS_BYTE))
				Logging::writeLog("[BASS] [Error] Failed to set channel position: " + std::to_string(BASS_ErrorGetCode()));
		}

		/// <summary>
		/// Get a float* repersentation of the channel
		/// </summary>
		/// <param name="sampleLength">The length</param>
		/// <returns>The sample</returns>
		float* ReturnSongSample(int* sampleLength)
		{
			if (decode == -1)
				decode = BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE);

			int leng = BASS_ChannelGetLength(decode, BASS_POS_BYTE);

			float samples[4096];

			leng = BASS_ChannelGetData(decode, samples, BASS_DATA_FFT4096 | BASS_DATA_AVAILABLE);
			*sampleLength = leng;

			if (BASS_ErrorGetCode() != 0) {
				Logging::writeLog("[BASS] [Error] Failed to get Song Samples, Error " + std::to_string(BASS_ErrorGetCode()));
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
				Logging::writeLog("[BASS] [Error] Failed to return samples, Error " + std::to_string(BASS_ErrorGetCode()));
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
			if (!BASS_ChannelSetAttribute(id, BASS_ATTRIB_TEMPO_FREQ, SampleRate() * _rate))
				Logging::writeLog("[BASS] [Error] Failed to set channel rate: " + std::to_string(BASS_ErrorGetCode()));
		}

		/// <summary>
		/// Convert the channel to a Effects Channel
		/// </summary>
		void ConvertToFX()
		{
			if (id == -1)
				return;
			BASS_ChannelFree(id);
			id = BASS_FX_TempoCreate(BASS_StreamCreateFile(false, path.c_str(), 0, 0, BASS_STREAM_DECODE), BASS_FX_FREESOURCE | BASS_FX_TEMPO_ALGO_SHANNON);


			BASS_ChannelSetAttribute(id, BASS_ATTRIB_TEMPO_OPTION_SEQUENCE_MS, 0);
			BASS_ChannelSetAttribute(id, BASS_ATTRIB_TEMPO_OPTION_USE_QUICKALGO, 1);
			BASS_ChannelSetAttribute(id, BASS_ATTRIB_TEMPO_OPTION_OVERLAP_MS, 4);
			BASS_ChannelSetAttribute(id, BASS_ATTRIB_TEMPO_OPTION_USE_AA_FILTER, 1);

		}

		/// <summary>
		/// Set the current volume of the current channel
		/// </summary>
		/// <param name="vol">The volume to set</param>
		void SetVolume(float vol)
		{
			if (id == -1)
				return;
			if (vol < 0.05f)
				volume = 0;
			else
				volume = sqrt(vol); // better curve

			if (volume > 1.0f)
				volume = 1.0f;
			if (volume < 0.0f)
				volume = 0.0f;

			if (!BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, vol))
				Logging::writeLog("[BASS] [Error] Failed to set channel volume: " + std::to_string(BASS_ErrorGetCode()));
		}

		bool operator==(const Channel& other) {
			return (name == other.name) || (id == other.id);
		}
	};
}

#endif // !CHANNEL_H