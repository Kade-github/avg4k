#pragma once
#include "includes.h"
namespace Average4k::Lua::Menu
{
	struct note
	{
	public:
		float beat;
		int lane;
		int type;

		note()
		{
			beat = 0;
			lane = 0;
			type = 0;
		}
	};

	struct stopPoint {
	public:
		float startBeat;
		float startTime;
		float length;
		float endBeat;
		float endTime;
		stopPoint()
		{
			startBeat = 0;
			startTime = 0;
			length = 0;
			endBeat = 0;
			endTime = 0;
		}
	};

	struct timingPoint
	{
	public:
		float startBeat;
		float endBeat;
		float startTime;
		float endTime;
		float length;

		float bpm;

		timingPoint()
		{
			startBeat = 0;
			endBeat = 0;
			startTime = 0;
			endTime = 0;
			length = 0;
			bpm = 0;
		}
	};

	struct difficulty
	{
	public:
		std::string name;
		std::string charter;
		int difficultyRating;
		std::vector<note> notes;

		difficulty()
		{
			name = "";
			charter = "";
			difficultyRating = 0;
			notes = {};
		}
	};

	struct chart
	{
	public:
		std::string songArtist;
		std::string songTitle;
		std::string songSubtitle;
		std::string songCredit;
		std::string songBanner;
		std::string songBackground;
		std::string songFile;
		float songOffset;
		int chartType;
		float previewStart;
		std::vector<timingPoint> timingPoints;
		std::vector<stopPoint> stopPoints;
		std::vector<difficulty> difficulties;
		std::string path;

		chart()
		{
			songArtist = "";
			songTitle = "";
			songSubtitle = "";
			songCredit = "";
			songBanner = "";
			songBackground = "";
			songFile = "";
			songOffset = 0;
			chartType = 0;
			previewStart = 0;
			timingPoints = {};
			stopPoints = {};
			difficulties = {};
		}
	};
}