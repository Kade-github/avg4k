/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_CHART_FILE_H
#define _AVG4K_CHART_FILE_H

#include <AvgEngine/Utils/Logging.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "Metadata.h"
#include "StopPoint.h"
#include "TimingPoint.h"
#include "Difficulty.h"
#include "ScrollPoint.h"
#include "SpeedPoints.h"

#define MAX(a,b) (((a)>(b))?(a):(b)) // for some reason std::max is being defined by another definition of max so we just do our own fuck you.

namespace Average4k::Data
{
	class ChartFile
	{
	public:
		bool isValid = false;

		std::wstring path = L"";
		Chart::Metadata metadata = {};

		std::vector<Chart::TimingPoint> timingPoints = {};
		std::vector<Chart::StopPoint> stopPoints = {};
		std::vector<Chart::ScrollPoint> scrollPoints = {};
		std::vector<Chart::SpeedPoint> speedPoints = {};

		std::vector<Chart::Difficulty> difficulties = {};

		ChartFile() = default;

		virtual void Parse(std::wstring path, bool metadataOnly) {};
		virtual void Write(std::wstring path) {
			AvgEngine::Logging::writeLog("[Error] ChartFile::Write() is not implemented for chart type: " + std::to_string(metadata.type));
		};

		void ConstructTimings();

		Chart::TimingPoint GetTimingPoint(float beat)
		{
			Chart::TimingPoint tp{};

			for (Chart::TimingPoint& _tp : timingPoints)
			{
				if (_tp.startBeat <= beat && _tp.endBeat > beat)
				{
					tp = _tp;
					break;
				}
			}
			return tp;
		}

		Chart::TimingPoint GetTimingPointAfter(Chart::TimingPoint p, float beat)
		{
			if (p.endBeat > beat)
				return p;
			Chart::TimingPoint tp{};
			for (Chart::TimingPoint& _tp : timingPoints)
			{
				if (_tp.endBeat < p.endBeat)
					continue;
				if (_tp.startBeat <= beat && _tp.endBeat > beat)
				{
					tp = _tp;
					break;
				}
			}
			return tp;
		}

		Chart::StopPoint GetStopPoint(float beat)
		{
			Chart::StopPoint sp{};
			for (Chart::StopPoint& _sp : stopPoints)
			{
				if (_sp.startBeat <= beat)
				{
					sp = _sp;
				}
			}
			return sp;
		}

		Chart::StopPoint GetStopPointTime(float time)
		{
			Chart::StopPoint sp{};
			for (Chart::StopPoint& _sp : stopPoints)
			{
				if (_sp.startTimestamp <= time)
				{
					sp = _sp;
				}
			}
			return sp;
		}



		float GetTimeFromBeat(float beat)
		{
			float time = 0;
			for (Chart::TimingPoint& tp : timingPoints)
			{
				if (tp.startBeat <= beat && tp.endBeat > beat)
				{
					Chart::StopPoint sp = GetStopPoint(beat);
					float bigBeat = tp.startBeat;
					float yo = tp.startTimestamp;
					if (sp.startBeat > tp.startBeat)
					{
						bigBeat = sp.startBeat;
						yo = sp.startTimestamp + sp.length;
					}

					const float b = (beat - bigBeat) / (tp.bpm / 60.0f);
					return (yo + b);
				}
			}
			return time;
		}

		float GetBeatFromTime(float time)
		{
			float beat = 0;
			float lol = time;
			if (lol < 0)
				lol = timingPoints[0].startTimestamp;
			for (Chart::TimingPoint& tp : timingPoints)
			{
				if (tp.startTimestamp <= lol && tp.endTimestamp > lol)
				{
					Chart::StopPoint sp = GetStopPointTime(time);
					float stopEndTime = sp.startTimestamp + sp.length;
					if (time > sp.startTimestamp && time < stopEndTime)
						return sp.startBeat;
					float larger = MAX(stopEndTime, tp.startTimestamp);
					float beast = tp.startBeat;
					if (tp.startTimestamp < stopEndTime)
						beast = sp.startBeat;
					float cb = beast + ((time - larger) * (tp.bpm / 60.0f));
					return cb;
				}
			}
			return beat;
		}

		float GetTimeFromBeat(float beat, Chart::TimingPoint tp)
		{
			float time = 0;
			if (tp.startBeat <= beat && tp.endBeat > beat)
			{
				const float b = (beat - tp.startBeat) / (tp.bpm / 60.0f);
				return (tp.startTimestamp + b);
			}
			return time;
		}

		float GetBeatFromTime(float time, Chart::TimingPoint tp)
		{
			float beat = 0;
			if (tp.startTimestamp <= time && tp.endTimestamp > time)
			{
				return tp.startBeat + ((time - tp.startTimestamp) * (tp.bpm / 60.0f));
			}
			return beat;
		}

		Chart::ScrollPoint GetScrollPoint(float beat)
		{
			Chart::ScrollPoint sp{};
			for (Chart::ScrollPoint& _sp : scrollPoints)
			{
				if (_sp.startBeat <= beat && _sp.endBeat > beat)
				{
					sp = _sp;
				}
			}
			return sp;
		}

		Chart::SpeedPoint GetSpeedPoint(float beat)
		{
			Chart::SpeedPoint sp{};
			for (Chart::SpeedPoint& _sp : speedPoints)
			{
				if (_sp.startBeat <= beat && _sp.endBeat > beat)
				{
					sp = _sp;
				}
			}
			return sp;
		}
	};
}

#endif