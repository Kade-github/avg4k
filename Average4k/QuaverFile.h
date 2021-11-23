#pragma once
#include <sstream>
#include <string>
#include <fstream>
#include <filesystem>
#include "Chart.h"

class QuaverFile
{
	public:
        chartMeta meta;
        note currentWorkingNote;
		QuaverFile();

		chartMeta returnChart(std::string path);

		bool ends_with(std::string const& value, std::string const& ending)
		{
			if (ending.size() > value.size()) return false;
			return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
		}

		float getTimeFromBeatOffset(float beat, bpmSegment seg) {
			return seg.startTime + ((beat - seg.startBeat) / (seg.bpm / 60));
		}

		float getBeatFromTimeOffset(float timestamp, bpmSegment seg) {
			return seg.startBeat + (((timestamp / 1000) - (seg.startTime)) * (seg.bpm / 60));
		}

        bpmSegment getSegmentFromTime(float time, std::vector<bpmSegment>& toSearch) {
            bpmSegment seg;
            seg.bpm = toSearch[0].bpm;
            seg.startBeat = 0;
            seg.startTime = 0;
            seg.endBeat = INT_MAX;
            seg.length = INT_MAX;

            for (int i = 0; i < toSearch.size(); i++) {
                bpmSegment segment = toSearch[i];
                if (time >= segment.startTime * 1000.f && time < (segment.startTime + segment.length) * 1000.f)
                    seg = segment;
            }


            return seg;
        }

        bpmSegment getSegmentFromBeat(float beat, std::vector<bpmSegment>& toSearch)
        {
            bpmSegment seg;
            seg.bpm = toSearch[0].bpm;
            seg.startBeat = 0;
            seg.startTime = 0;
            seg.endBeat = INT_MAX;
            seg.length = INT_MAX;

            for (int i = 0; i < toSearch.size(); i++) {
                bpmSegment segment = toSearch[i];
                if (beat >= segment.startBeat && beat < segment.endBeat)
                    seg = segment;
            }


            return seg;
        }
};

