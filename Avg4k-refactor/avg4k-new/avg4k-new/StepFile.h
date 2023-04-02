#pragma once
#include "includes.h"
#include "ChartFile.h"
#include "StringTools.h"
namespace Average4k::Chart
{
	class StepFile : public ChartFile {
	public:
		StepFile(std::string _path) : ChartFile(_path) {}

		void ParseBpms(std::string line, int ln, int* state)
		{
			std::string t = line;
			if (AvgEngine::Utils::StringTools::Contains(line, ":"))
			{
				// Get the part after the colon
				t = line.substr(line.find(":") + 1);
			}

			// remove semicolon if there is one
			if (t.ends_with(";"))
				t = t.substr(0, t.length() - 1);

			// Split the string by ","'s and then split that string by "="'s
			std::vector<std::string> split = AvgEngine::Utils::StringTools::Split(t, ",");
			for (std::string s : split)
			{
				std::vector<std::string> split2 = AvgEngine::Utils::StringTools::Split(s, "=");
				if (split2.size() != 2)
					continue;
				// Get the bpm and the beat
				float bpm = std::stof(split2[0]);
				float beat = std::stof(split2[1]);

				TimingPoint p;
				p.Bpm = bpm;
				p.StartBeat = beat;
				p.EndBeat = INT_MAX;
				p.StartTimestamp = chartMetadata.Song_Offset;
				p.EndTimestamp = INT_MAX;
				p.LengthTimestamp = INT_MAX;
				
				// Get the start time and end time if there are any timing points
				if (chartMetadata.TimingPoints.size() > 0)
				{
					TimingPoint& last = chartMetadata.TimingPoints[chartMetadata.TimingPoints.size() - 1];
					p.StartTimestamp = GetTimeFromBeat(beat);
					last.EndBeat = beat;
					last.EndTimestamp = p.StartTimestamp;
					last.LengthTimestamp = last.EndTimestamp - last.StartTimestamp;
				}

				// Add the bpm to the vector
				chartMetadata.TimingPoints.push_back(p);
			}

			// if theres a semicolon at the end of a line, we're done with the bpms
			if (line.ends_with(";"))
				state = 0;
		}

		void ParseStops(std::string line, int ln, int* state)
		{
			std::string t = line;
			if (AvgEngine::Utils::StringTools::Contains(line, ":"))
			{
				// Get the part after the colon
				t = line.substr(line.find(":") + 1);
			}

			// remove semicolon if there is one
			if (t.ends_with(";"))
				t = t.substr(0, t.length() - 1);

			// Split the string by ","'s and then split that string by "="'s
			std::vector<std::string> split = AvgEngine::Utils::StringTools::Split(t, ",");
			for (std::string s : split)
			{
				std::vector<std::string> split2 = AvgEngine::Utils::StringTools::Split(s, "=");
				if (split2.size() != 2)
					continue;
				// Get the beat and length
				float beat = std::stof(split2[0]);
				float length = std::stof(split2[1]);
				// Create a stop point object and add it
				StopPoint p;
				p.StartBeat = beat;
				p.StartTimestamp = GetTimeFromBeat(beat);
				p.EndTimestamp = p.StartTimestamp + length;
				p.EndBeat = GetBeatFromTime(p.EndTimestamp);
				p.Length = length;

				// Add it to the list
				chartMetadata.Stops.push_back(p);
			}


			// if theres a semicolon at the end of a line, we're done with the stops
			if (line.ends_with(";"))
				state = 0;
		}

		void ParseNotes(std::string line, int ln, int* state, Difficulty* diff)
		{
			static int measureIndex = 0;
			static std::vector<std::string> measure;

			if (AvgEngine::Utils::StringTools::Contains(line, ":"))
			{
				// remove leading/trailing whitespace
				std::string t = AvgEngine::Utils::StringTools::Trim(line);
				// remove the last character (the colon)
				t = t.substr(0, t.length() - 1);

				switch (ln)
				{
				case 1:
					if (t != "dance-single")
						good = false;
					break;
				case 3:
					diff->Name = t;
					break;
				case 2:
					diff->Charter = t;
					break;
				case 4:
					diff->DifficultyRating = std::stoi(t);
					break;
				}
			}
			else
			{
				// if theres a comma at the end of the line, we're done with the measure
				if (line.ends_with(","))
				{
					// Magic numbers, woohoo!!
					// Basically, this generates the length of rows for the entire measure
					float lengthInRows = 192 / (measure.size());
					float beat = 0;

					int rowIndex = 0;

					for (int i = 0; i < measure.size(); i++) {
						// This calculates what the beat is using magic numbers!!! (its some music theory bullshit I dunno)
						float noteRow = (measureIndex * 192) + (lengthInRows * rowIndex);
						beat = noteRow / 48;

						for (int n = 0; n < measure[i].size(); n++) {
							char note = measure[i][n];

							if (note == '0') // skip empty space
								continue;

							Note newNote{};
							newNote.Beat = beat;
							newNote.Lane = n;
							newNote.Row = noteRow;
							switch (note)
							{
							default:
								newNote.Type = NoteType_Tap;
								break;
							case '2':
								newNote.Type = NoteType_Head;
								break;
							case '3':
								newNote.Type = NoteType_Tail;
								break;
							case '4':
								newNote.Type = NoteType_Head;
								break;
							case 'F':
								newNote.Type = NoteType_Fake;
								break;
							case 'M':
								newNote.Type = NoteType_Mine;
								break;
							}
							
							// Add the note to the list
							diff->Notes.push_back(newNote);
						}
						rowIndex++;
					}

					measureIndex++;
					measure.clear();
				}
				else
				{
					// otherwise we keep adding too the measure
					measure.push_back(line);
				}

				// if theres a semicolon at the end of the line, we're done with the notes
				if (line.ends_with(";"))
				{
					state = 0;
					measureIndex = 0;
					measure.clear();
					chartMetadata.Difficulties.push_back(*diff);
					diff = {};
				}
			}
		}


		void Parse() override
		{
			std::ifstream infile(path);

			if (!infile.good())
			{
				AvgEngine::Logging::writeLog("[StepFile] [Error] File not found: " + path);
				return;
			}

			good = true;

			// state
			int state = 0;
			// 0 = header
			// 1 = bpm
			// 2 = stops
			// 3 = notes

			Difficulty currentDiff{};

			int lineNumber = 0;

			bool skip = false;

			std::string line;
			while (std::getline(infile, line)) {
				switch (state)
				{
					case 0:
						// We reset the line number, because we want each section to be 0 indexed.
						// States are really just used in this switch case, thats why they exist.
						// Functions seperate the logic for each section.

						if (line == "#BPMS:")
						{
							lineNumber = 0;
							state = 1;
							ParseBpms(line, lineNumber, &state);
							break;
						}
						else if (line == "#STOPS:")
						{
							lineNumber = 0;
							state = 2;
							ParseStops(line, lineNumber, &state);
							break;
						}
						else if (line == "#NOTES:")
						{
							lineNumber = 0;
							state = 3;
							ParseNotes(line, lineNumber, &state, &currentDiff);
							break;
						}
						else
						{
							if (!AvgEngine::Utils::StringTools::Contains(line, ":"))
								continue;

							// Simple key value pairs, if statements are ugly but its whatever.

							std::string key = line.substr(0, line.find(":"));
							std::string value = line.substr(line.find(":") + 1, line.length() - 1); // -1 accounts for semicolon
							if (key == "#TITLE")
								chartMetadata.Song_Title = value;
							else if (key == "#ARTIST")
								chartMetadata.Song_Artist = value;
							else if (key == "#SUBTITLE")
								chartMetadata.Song_Subtitle = value;
							else if (key == "#CREDIT")
								chartMetadata.Song_Credit = value;
							else if (key == "#BANNER")
								chartMetadata.Song_Banner = value;
							else if (key == "#BACKGROUND")
								chartMetadata.Song_Background = value;
							else if (key == "#MUSIC")
								chartMetadata.Song_File = value;
							else if (key == "#OFFSET")
								chartMetadata.Song_Offset = std::stof(value);
						}
						break;
					case 1:
						ParseBpms(line, lineNumber, &state);
						break;
					case 2:
						ParseStops(line, lineNumber, &state);
						break;
					case 3:
						if (!good)
						{
							good = true;
							skip = true;
						}
						if (!skip)
							ParseNotes(line, lineNumber, &state, &currentDiff);
						else
							if (line.ends_with(";"))
							{
								state = 0;
								skip = false;
							}
						break;
				}
				lineNumber++;
			}

		}
	};
}