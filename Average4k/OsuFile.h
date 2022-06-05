#pragma once
#include "Chart.h"
class OsuFile
{
private:
	enum OsuHeaderType {
		OSUHEADER_General = 0,
		OSUHEADER_Editor = 1,
		OSUHEADER_Metadata = 2,
		OSUHEADER_Difficulty = 3,
		OSUHEADER_Events = 4,
		OSUHEADER_TimingPoints = 5,
		OSUHEADER_HitObjects = 6,
		OSUHEADER_None = -1
	};
public:
	chartMeta meta;

	OsuFile(std::string path);

	bool ends_with(std::string const& value, std::string const& ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}
	OsuHeaderType getHeaderFromString(std::string s);
	difficulty generateDiff(std::string file, chartMeta* toAdd);
	float getBeatFromTime(float timestamp, bpmSegment seg) {
		float result = seg.startBeat + (((((timestamp - (0.034 * 1000)) / 1000) - ((seg.startTime / 1000) - (meta.chartOffset))) * (seg.bpm / 60)));
		return result;
	}

	bpmSegment getSegmentFromTime(float time) {
		bpmSegment seg;
		seg.bpm = meta.bpms[0].bpm;
		seg.startBeat = 0;
		seg.startTime = 0;
		seg.endBeat = INT_MAX;
		seg.length = INT_MAX;

		for (int i = 0; i < meta.bpms.size(); i++) {
			bpmSegment segment = meta.bpms[i];
			if ((time - (0.034 * 1000)) >= segment.startTime && (time - (0.034 * 1000)) < ((segment.startTime - (0.034 * 1000)) + segment.length))
				seg = segment;
		}


		return seg;
	}
};

