/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "ChartFile.h"

void Average4k::Data::ChartFile::ConstructTimings()
{
	using namespace Average4k::Data::Chart;

	int tpIndex = 1;
	int spIndex = 0;
	if (timingPoints.size() > 1)
	{
		if (stopPoints.size() > 1)
		{
			while (tpIndex < timingPoints.size() || spIndex < stopPoints.size())
			{
				TimingPoint& prevTp = timingPoints[tpIndex - 1];

				if (tpIndex == timingPoints.size())
				{
					float cts = 0;
					while (spIndex < stopPoints.size())
					{
						StopPoint& sp = stopPoints[spIndex];
						sp.startTimestamp = (prevTp.startTimestamp + (sp.startBeat - prevTp.startBeat) / (prevTp.bpm / 60)) + cts;
						cts += sp.length;
						spIndex++;
					}
					continue;
				}

				if (spIndex == stopPoints.size())
				{
					while (tpIndex < timingPoints.size())
					{
						TimingPoint& tp = timingPoints[tpIndex];
						prevTp.endTimestamp += (prevTp.endBeat - prevTp.startBeat) / (prevTp.bpm / 60);
						tp.startTimestamp = prevTp.endTimestamp;
						tp.endTimestamp = tp.startTimestamp;
						tpIndex++;
						if (tpIndex == timingPoints.size())
						{
							tp.endTimestamp = INT_MAX;
							tp.endBeat = INT_MAX;
						}
					}
					continue;
				}

				TimingPoint& tp = timingPoints[tpIndex];
				StopPoint& sp = stopPoints[spIndex];

				prevTp.endBeat = tp.startBeat;

				if (sp.startBeat < prevTp.endBeat)
				{
					sp.startTimestamp = prevTp.endTimestamp + (sp.startBeat - prevTp.startBeat) / (prevTp.bpm / 60);
					prevTp.endTimestamp += sp.length;
					spIndex++;
				}
				else
				{
					prevTp.endTimestamp += (prevTp.endBeat - prevTp.startBeat) / (prevTp.bpm / 60);
					tp.startTimestamp = prevTp.endTimestamp;
					tp.endTimestamp = tp.startTimestamp;
					tpIndex++;
				}

			}
		}
		else
		{
			while (tpIndex < timingPoints.size())
			{
				TimingPoint& prevTp = timingPoints[tpIndex - 1];
				TimingPoint& tp = timingPoints[tpIndex];
				prevTp.endBeat = tp.startBeat;
				prevTp.endTimestamp = prevTp.startTimestamp + (prevTp.endBeat - prevTp.startBeat) / (prevTp.bpm / 60);
				tp.startTimestamp = prevTp.endTimestamp;
				tpIndex++;
				if (tpIndex == timingPoints.size())
				{
					tp.endTimestamp = INT_MAX;
					tp.endBeat = INT_MAX;
				}
			}
		}
	}

	if (timingPoints.size() != 0)
	{
		timingPoints.back().endTimestamp = INT_MAX;
		timingPoints.back().endBeat = INT_MAX;
	}

	// SpeedPoints
	for (int i = 0; i < speedPoints.size(); i++)
	{
		SpeedPoint& sp = speedPoints[i];
		sp.startTimestamp = GetTimeFromBeat(sp.startBeat);
		if (sp.type == 0)
		{
			sp.endBeat = sp.startBeat + sp.delay;
			sp.endTimestamp = GetTimeFromBeat(sp.endBeat);
		}
		else
		{
			sp.endTimestamp = sp.startTimestamp + sp.delay;
			sp.endBeat = GetBeatFromTime(sp.endTimestamp);
		}

		sp.spdId = i;
	}
}
