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
		float StartTimestamp = -1;
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

		bool isSteam;
	};

	class ChartFile
	{
	public:
		Metadata chartMetadata;

		bool good = false;

		std::string path;
		std::string folder;

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
				if (_sp.StartBeat <= beat)
				{
					sp = _sp;
				}
			}
			return sp;
		}

		StopPoint GetStopPointTime(float time)
		{
			StopPoint sp{};
			for (StopPoint& _sp : chartMetadata.Stops)
			{
				if (_sp.StartTimestamp <= time)
				{
					sp = _sp;
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
					Chart::StopPoint sp = GetStopPoint(beat);
					float bigBeat = tp.StartBeat;
					float yo = tp.StartTimestamp;
					if (sp.StartBeat > tp.StartBeat)
					{
						bigBeat = sp.StartBeat;
						yo = sp.StartTimestamp + sp.Length;
					}

					const float b = (beat - bigBeat) / (tp.Bpm / 60.0f);
					return (yo + b);
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
					Chart::StopPoint sp = GetStopPointTime(time);
					float stopEndTime = sp.StartTimestamp + sp.Length;
					if (time > sp.StartTimestamp && time < stopEndTime)
						return sp.StartBeat;
					float larger = std::max(stopEndTime, tp.StartTimestamp);
					float beast = tp.StartBeat;
					if (tp.StartTimestamp < stopEndTime)
						beast = sp.StartBeat;
					float cb = beast + ((time - larger) * (tp.Bpm / 60.0f));
					return cb;
				}
			}
			return beat;
		}

		float GetTimeFromBeat(float beat, TimingPoint tp)
		{
			float time = 0;
			if (tp.StartBeat <= beat && tp.EndBeat > beat)
			{
				const float b = (beat - tp.StartBeat) / (tp.Bpm / 60.0f);
				return (tp.StartTimestamp + b);
			}
			return time;
		}

		float GetBeatFromTime(float time, TimingPoint tp)
		{
			float beat = 0;
			if (tp.StartTimestamp <= time && tp.EndTimestamp > time)
			{
				return tp.StartBeat + ((time - tp.StartTimestamp) * (tp.Bpm / 60.0f));
			}
			return beat;
		}

	};
}