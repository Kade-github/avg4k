/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_TIME_TO_SCREEN_H
#define AVG4K_TIME_TO_SCREEN_H

#pragma once

#define PIXELS_BETWEEN_BEATS 128

namespace Average4k::Helpers
{
	class TimeToScreen
	{
	public:
		/// <summary>
		/// Calculate the Y offset of a note based on the (desired) speed and difference in beats
		/// </summary>
		/// <param name="speed">The speed as a BPM</param>
		/// <param name="differenceInBeats">The difference between the current beat and the beat of the note</param>
		/// <returns>The calculated Y offset</returns>
		static float YOffset(double speed, double differenceInBeats, float zoomFactor = 1.0f)
		{
			double bps = (speed / 60);
			return ((bps * differenceInBeats) * PIXELS_BETWEEN_BEATS) * zoomFactor;
		}

		static float YOffsetXMod(double speed, double differenceInBeats, float zoomFactor = 1.0f)
		{
			return ((differenceInBeats * PIXELS_BETWEEN_BEATS) * speed) * zoomFactor;
		}

		/// <summary>
		/// Calculate what the beat would be from a given Y offset
		/// </summary>
		/// <param name="speed">The speed as a BPM</param>
		/// <param name="offset">The offset from the current beat (in pixels)</param>
		/// <returns>The calculated beat</returns>
		static double YOffsetToBeat(double speed, int offset, float zoomFactor = 1.0f)
		{
			double bps = (speed / 60);
			return (double)((offset / zoomFactor) / (bps * PIXELS_BETWEEN_BEATS));
		}

		static double YOffsetToBeatXMod(double speed, int offset, float zoomFactor = 1.0f)
		{
			return (double)((offset / zoomFactor) / PIXELS_BETWEEN_BEATS);
		}
	};
}

#endif