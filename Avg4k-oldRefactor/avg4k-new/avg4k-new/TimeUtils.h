#pragma once
#include "includes.h"
namespace Average4k::Utils
{
	class TimeUtils
	{
	public:
		/**
		 * \brief A helper function to convert beats to time
		 * \param bpm The bpm
		 * \param beat The beat to convert
		 * \param startBeat The start beat (if needed, to take in account bpm changes)
		 * \param startTime The start time (if needed, to take in account bpm changes)
		 * \return Time
		 */
		static float ConvertBeatToTime(double bpm, double beat, double startBeat = 0, float startTime = 0)
		{
			const float b = (static_cast<float>(beat) - static_cast<float>(startBeat)) / (static_cast<float>(bpm) / 60.0f);
			return (startTime + b);
		}

		/**
		 * \brief A helper function to convert time to beats
		 * \param bpm The bpm
		 * \param time The time
		 * \param startBeat The start beat (if needed, to take in account bpm changes)
		 * \param startTime The start time (if needed, to take in account bpm changes)
		 * \return Beats
		 */
		static double ConvertTimeToBeat(double bpm, float time, double startBeat = 0, float startTime = 0)
		{
			return startBeat + ((static_cast<double>((time)) - static_cast<double>((startTime))) * (bpm / 60));
		}
	};
}