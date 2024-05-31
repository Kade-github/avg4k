/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_WAVEFORM_DATA_H
#define AVG4K_WAVEFORM_DATA_H

#pragma once

#include <AvgEngine/External/Bass/BASS.h>
#include <map>

namespace Average4k::Helpers
{
	class WaveformData
	{
	public:
		static std::map<float,float> GetWaveform(AvgEngine::Audio::Channel* c, int waveform_precision = 100)
		{
			if (c->decode == -1)
			{
				AvgEngine::Logging::writeLog("[Error] Cannot get waveform data from a channel that has not been decoded yet.");
				return {};
			}
			DWORD msLength = BASS_ChannelBytes2Seconds(c->decode, BASS_ChannelGetLength(c->decode, BASS_POS_BYTE)) * 1000;
			DWORD length = BASS_ChannelGetLength(c->decode, BASS_POS_BYTE);

			float* waveformData = (float*)malloc(length * sizeof(float));

			length = BASS_ChannelGetData(c->decode, waveformData, length);

			std::map<float, float> _ReadableWaveFormData = {};

			for (int ms = 0; ms < msLength; ms += waveform_precision)
			{
				float seconds = float(ms) / 1000.0f;
				QWORD index = BASS_ChannelSeconds2Bytes(c->id, max(0.0f, seconds)) / 2;

				if (index < length / sizeof(int))
				{
					float right = waveformData[index + 1];
					float left = waveformData[index];

					_ReadableWaveFormData[seconds] = (right + left) / 2;
				}
			}

			free(waveformData);

			return _ReadableWaveFormData;
		}
	};
}

#endif 