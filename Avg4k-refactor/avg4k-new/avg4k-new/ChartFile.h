#pragma once
#include "includes.h"
namespace Average4k::Chart
{
	enum NoteType {
		NoteType_Tap = 1,
		NoteType_Head = 2,
		NoteType_Tail = 3,
		NoteType_Mine = 4,
		NoteType_Fake = 5
	};

	struct Note {
		float Beat;
		int Lane;
		int Row;
		int Type;
	};

	struct TimingPoint {
		float Bpm = 0;
		float StartBeat = -1;
		float EndBeat = -1;
		float StartTimestamp = -1;
		float EndTimestamp = -1;
		float LengthTimestamp = -1;
	};

	struct StopPoint {
		float StartBeat = -1;
		float EndBeat = -1;
		float StartTimestamp = -1;
		float EndTimestamp = -1;
		float Length = -1;

	};

	struct Difficulty {
		std::string Name{};
		std::string Charter{};
		std::vector<Note> Notes = {};
		int DifficultyRating = 0;
	};


	struct Metadata {
		std::string Song_Artist;
		std::string Song_Title;
		std::string Song_Subtitle;
		std::string Song_Credit;
		std::string Song_Banner;
		std::string Song_Background;
		std::string Song_File;
		float Song_Offset;

		int Chart_Type;

		float previewStart = 0;

		std::vector<TimingPoint> TimingPoints;
		std::vector<StopPoint> Stops;
		std::vector<Difficulty> Difficulties;
	};

	class ChartFile
	{
	public:
		Metadata chartMetadata;

		bool good = false;

		std::string path;

		ChartFile(std::string _path)
		{
			path = _path;
		}

		~ChartFile()
		{

		}

		virtual void Parse()
		{

		}

		// Helper Functions

		TimingPoint GetTimingPoint(float beat)
		{
			TimingPoint tp{};
			for (TimingPoint& _tp : chartMetadata.TimingPoints)
			{
				if (_tp.StartBeat <= beat && _tp.EndBeat > beat)
				{
					tp = _tp;
					break;
				}
			}
			return tp;
		}

		TimingPoint GetTimingPointAfter(TimingPoint p, float beat)
		{
			if (p.EndBeat > beat)
				return p;
			TimingPoint tp{};
			for (TimingPoint& _tp : chartMetadata.TimingPoints)
			{
				if (_tp.EndBeat < p.EndBeat)
					continue;
				if (_tp.StartBeat <= beat && _tp.EndBeat > beat)
				{
					tp = _tp;
					break;
				}
			}
			return tp;
		}

		StopPoint GetStopPoint(float beat)
		{
			StopPoint sp{};
			for (StopPoint& _sp : chartMetadata.Stops)
			{
				if (_sp.StartBeat <= beat && _sp.EndBeat > beat)
				{
					sp = _sp;
					break;
				}
			}
			return sp;
		}

		StopPoint GetStopPointAfter(StopPoint p, float beat)
		{
			if (p.EndBeat > beat)
				return p;
			StopPoint sp{};
			for (StopPoint& _sp : chartMetadata.Stops)
			{
				if (_sp.EndBeat < p.EndBeat)
					continue;
				if (_sp.StartBeat <= beat && _sp.EndBeat > beat)
				{
					sp = _sp;
					break;
				}
			}
			return sp;
		}

		float GetTimeFromBeat(float beat)
		{
			float time = 0;
			for (TimingPoint& tp : chartMetadata.TimingPoints)
			{
				if (tp.StartBeat <= beat && tp.EndBeat > beat)
				{
					float _beat = (beat - tp.StartBeat) / (tp.Bpm);
					time = (tp.StartTimestamp + _beat) - (chartMetadata.Song_Offset);
					break;
				}
			}
			return time;
		}

		float GetBeatFromTime(float time)
		{
			float beat = 0;
			for (TimingPoint& tp : chartMetadata.TimingPoints)
			{
				if (tp.StartTimestamp <= time && tp.EndTimestamp > time)
				{
					float _time = (time - tp.StartTimestamp) - (chartMetadata.Song_Offset);
					beat = (tp.StartBeat + _time) * (tp.Bpm / 60);
					break;
				}
			}
			return beat;
		}

		float GetTimeFromBeat(float beat, TimingPoint tp)
		{
			float time = 0;
			if (tp.StartBeat <= beat && tp.EndBeat > beat)
			{
				float _beat = (beat - tp.StartBeat) / (tp.Bpm);
				time = (tp.StartTimestamp + _beat) - (chartMetadata.Song_Offset);
			}
			return time;
		}

		float GetBeatFromTime(float time, TimingPoint tp)
		{
			float beat = 0;
			if (tp.StartTimestamp <= time && tp.EndTimestamp > time)
			{
				float _time = (time - tp.StartTimestamp) - (chartMetadata.Song_Offset);
				beat = (tp.StartBeat + _time) * (tp.Bpm / 60);
			}
			return beat;
		}
	};
}