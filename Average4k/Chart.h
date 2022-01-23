#pragma once
#include "includes.h"
#include <sstream>
#include <string>
#include <fstream>

struct bpmSegment {
	float startBeat;
	float startTime;
	float endBeat;
	float length; // in ms
	float bpm;
};

struct stopSegment {
	float length;
	float beat;
};

enum noteType {
	Note_Normal = 1,
	Note_Head = 2,
	Note_Tail = 3,
	Note_Mine = 5,
};

struct note {
	float beat;
	noteType type;
	int lane;

	// gameplay var
	bool played;
	bool killed;
};

struct difficulty
{
	std::string name;
	std::string charter;
	std::vector<note> notes;
};

class chartMeta {
	public:
		std::string songName;
		std::string audio;
		std::string folder;
		std::string background;
		std::vector<bpmSegment> bpms;
		std::vector<stopSegment> stops;
		std::vector<difficulty> difficulties;
		float chartOffset;
		int chartType = 0;
};

class Chart
{
	public:
		Chart(chartMeta m) { meta = m; };
		chartMeta meta;

		~Chart()
		{
			// not much lol
		}
		static std::vector < std::string > split(std::string str, char delimiter);

		float getTimeFromBeat(float beat, bpmSegment seg);
		float getTimeFromBeatOffset(float beat, bpmSegment seg);
		float getBeatFromTime(float timestamp, bpmSegment seg);
		float getBeatFromTimeOffset(float timestamp, bpmSegment seg);
		bpmSegment getSegmentFromTime(float timestamp);
		float getStopOffsetFromTime(float timestamp);
		float getStopOffsetFromBeat(float beat);
		bpmSegment getSegmentFromBeat(float beat);

		void destroy();
};

