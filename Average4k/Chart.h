#pragma once
#include "includes.h"
#include <sstream>
#include <string>
#include <fstream>

struct bpmSegment {
	float startBeat;
	float startTime;
	float endBeat;
	float length; // in seconds
	float bpm;
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
};

struct difficulty
{
	std::string name;
	std::string charter;
	std::vector<note>* notes;
};

class chartMeta {
	public:
		std::string songName;
		std::string audio;
		std::string folder;
		std::vector<bpmSegment>* bpms;
		std::vector<difficulty>* difficulties;
		float chartOffset;
		int chartType = 0;
};

class Chart
{
	public:
		Chart(chartMeta m) { meta = m; };
		chartMeta meta;

		static std::vector < std::string > split(std::string str, char delimiter);

		float getTimeFromBeat(float beat, bpmSegment seg);
		float getTimeFromBeatOffset(float beat, bpmSegment seg);
		float getBeatFromTime(float timestamp, bpmSegment seg);
		float getBeatFromTimeOffset(float timestamp, bpmSegment seg);
		bpmSegment getSegmentFromTime(float timestamp);
		bpmSegment getSegmentFromBeat(float beat);

		void destroy();
};

