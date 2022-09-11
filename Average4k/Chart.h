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
	float row = -99;
	noteType type;
	int lane;

	float connectedBeat = -1;

	// gameplay var
	bool played;
	bool killed;
};

struct info {
	float stream = 0;
	float jacks = 0;
	float jumpstream = 0;
	float chordjack = 0;
	float handstream = 0;
};


struct difficulty
{
	info info;
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
		std::string banner;
		std::string artist;
		std::string ext;
		std::vector<bpmSegment> bpms;
		std::vector<stopSegment> stops;
		std::vector<difficulty> difficulties;
		float chartOffset;
		float start = 0;
		int chartType = 0;
};


class Chart
{
	private:
		bpmSegment previouslyFound;
		bpmSegment nextSeg;
	public:



		float BASS_OFFSET;

		Chart() {};

		void getInfo()
		{
			for (difficulty& d : meta.difficulties) {
				float measures = 0;

				float streamMeasures = 0;
				float jumpstreamMeasures = 0;
				float chordjackMeasures = 0;
				float handstreamMeasures = 0;
				float jackMeasures = 0;

				int curMeasure = 0;
				float lastBeat = -1;

				float jumps = 0;
				float hands = 0;
				float singles = 0;
				float quads = 0;
				float jacks = 0;

				float allNotes = 0;

				std::map < int, note > stuff;

				for (note n : d.notes) {
					bool huh = false;

					if (n.type == Note_Tail || n.type == Note_Mine)
						continue;

					if (stuff.size() > 0) {
						for (const auto& [k, v] : stuff) {
							if (v.beat >= n.beat - 0.01 && v.beat < n.beat + 0.01)
								huh = true;
							else {
								switch (stuff.size()) {
								case 1:
									if ((n.beat - v.beat) <= .5 && v.lane == n.lane)
									{
										jacks++;
									}
									else
										singles++;
									break;
								case 2:
									jumps+= 2;
									break;
								case 3:
									hands+= 3;
									break;
								case 4:
									quads += 4;
									break;
								}
								stuff.clear();
								break;
							}

						}
					}

					stuff[n.lane] = n;

					if (lastBeat == -1)
						lastBeat = n.beat;

					if (lastBeat + 2 < n.beat && n.beat < lastBeat + 4) {
						curMeasure++;
						measures++;

						lastBeat = n.beat;

						float avgSingles = singles / allNotes;
						float avgJumps = jumps / allNotes;
						float avgHands = hands / allNotes;
						float avgQuads = quads / allNotes;
						float avgJacks = jacks / allNotes;

						if (avgSingles > 0.2 && (avgHands > 0.75 || avgQuads > 0.75) && jacks < 0.1)
							handstreamMeasures++;
						else if (avgJacks > 0 && (avgHands < 0.2 || avgJumps < 0.2))
							jackMeasures++;
						else if (avgSingles < 0.1 && (avgHands > 0.2 || avgJumps > 0.9 || avgQuads > 0.4))
							chordjackMeasures++;
						else if (avgJumps > 0 && avgSingles > 0)
							jumpstreamMeasures++;
						else if (avgSingles > 0)
							streamMeasures++;

						jumps = 0;
						hands = 0;
						singles = 0;
						quads = 0;
						jacks = 0;
						allNotes = 0;
					}
					else if (n.beat > lastBeat + 4)
					{
						curMeasure++;
						measures++;

						lastBeat = n.beat;
						jumps = 0;
						hands = 0;
						singles = 0;
						quads = 0;
						jacks = 0;
						allNotes = 0;
					}

					allNotes++;
				}

				info in;
				if (chordjackMeasures != 0)
					in.chordjack = chordjackMeasures / measures;
				if (handstreamMeasures != 0)
					in.handstream = handstreamMeasures / measures;
				if (streamMeasures != 0)
					in.stream = streamMeasures / measures;
				if (jackMeasures != 0)
					in.jacks = jackMeasures / measures;
				if (jumpstreamMeasures != 0)
					in.jumpstream = jumpstreamMeasures / measures;

				float sum = in.stream + in.jumpstream + in.chordjack + in.handstream + in.jacks;

				if (sum < 1)
				{
					if (in.stream > (in.jumpstream + in.chordjack + in.handstream + in.jacks))
						in.stream += (1 - sum);
					if (in.chordjack > (in.jumpstream + in.stream + in.handstream + in.jacks))
						in.chordjack += (1 - sum);
					if (in.handstream > (in.jumpstream + in.stream + in.chordjack + in.jacks))
						in.handstream += (1 - sum);
					if (in.jacks > (in.jumpstream + in.stream + in.chordjack + in.handstream))
						in.handstream += (1 - sum);
					if (in.jumpstream > (in.handstream + in.stream + in.chordjack + in.jacks))
						in.jumpstream += (1 - sum);
				}


				d.info = in;

			}
		}
		Chart(chartMeta m, bool checkformod = true) { 
			meta = m;
			
			if (std::filesystem::exists(m.folder + "/mod") && checkformod)
			{
				isModFile = true;
				pathToLua = m.folder + "/mod/mod.lua";
			}
			// mp3/other bass offsets

			if (m.ext == "mp3")
				BASS_OFFSET = 0.034f * 2.5f;
			else
				BASS_OFFSET = 0.034f;

			// get info

			getInfo();
		};
		chartMeta meta;

		~Chart()
		{
			// not much lol
		}
		static std::vector < std::string > split(std::string str, char delimiter);

		bool isModFile = false;

		std::string pathToLua = "";

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

