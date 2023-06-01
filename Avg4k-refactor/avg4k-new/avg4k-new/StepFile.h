#pragma once
#include "ChartFile.h"
#include <boost/lexical_cast.hpp>
#include <regex>
namespace Average4k::Chart
{
	class StepFile : public ChartFile {
		
		// Storage variable for the current working difficulty.
		Difficulty workingDiff{};

		// Storage variable if to skip the current difficulty
		bool skipDiff = false;

		// Storage variables for measures
		int currentMeasure = 0;
		std::vector<std::string> measure;

		void ParseStops(std::string& line, int* state)
		{
			// Clean up the line
			if (line.find(':') != std::string::npos) // remove the starting "#STOPS:" if it exists
				line = line.substr(line.find(':') + 1);

			if (line.empty())
				return;

			// check if ; exists
			bool bracket = line.back() == ';';

			// also formatted line, without a ";" at the end (IF IT EXISTS!)
			std::string fLine = line;
			if (bracket)
				fLine = fLine.substr(0, fLine.length() - 1);

			// Split the line via ","'s
			std::vector<std::string> split = AvgEngine::Utils::StringTools::Split(fLine, ",");
			for (auto& s : split)
			{
				// Split the line via "="'s
				std::vector<std::string> split2 = AvgEngine::Utils::StringTools::Split(s, "=");
				if (split2.size() != 2)
					continue;

				StopPoint p;
				p.StartBeat = std::stof(split2[0]);
				p.Length = std::stof(split2[1]);

				chartMetadata.Stops.push_back(p);
			}


			// if line ends with ";", set state to 0
			if (bracket)
				*state = 0;
		}

		void ParseBPMS(std::string& line, int* state)
		{
			// Clean up the line
			if (line.find(':') != std::string::npos) // remove the starting "#BPMS:" if it exists
				line = line.substr(line.find(':') + 1);

			if (line.empty())
				return;

			// check if ; exists
			bool bracket = line.front() == ';';

			// also formatted line, without a ";" at the end (IF IT EXISTS!)
			std::string fLine = line;
			if (bracket)
				fLine = fLine.substr(0, fLine.length() - 1);


			// Split the line via ","'s
			std::vector<std::string> split = AvgEngine::Utils::StringTools::Split(fLine, ",");
			for (auto& s : split)
			{
				// Split the line via "="'s
				std::vector<std::string> split2 = AvgEngine::Utils::StringTools::Split(s, "=");
				if (split2.size() != 2)
					continue;
				TimingPoint p;
				p.StartBeat = std::stof(split2[0]);
				p.Bpm = std::stof(split2[1]);
				p.EndBeat = 0;
				p.EndTimestamp = 0;
				p.LengthTimestamp = 0;
				if (chartMetadata.TimingPoints.size() == 0)
				{
					p.StartTimestamp = -chartMetadata.Song_Offset;
					p.EndTimestamp = -chartMetadata.Song_Offset;
				}

				chartMetadata.TimingPoints.push_back(p);
			}

			// if line ends with ";", set state to 0
			if (bracket)
				*state = 0;
		}

		void ParseNotes(std::string& line, int lineNumber, int* state)
		{
			// if it contains a ":" we are in a metadata section of the notes
			if (line.find(':') != std::string::npos)
			{
				std::string trim = AvgEngine::Utils::StringTools::Trim(line);
				switch (lineNumber)
				{
				case 1: // chart style
					// if its not a dance single, we dont fuck wit it.
					if (!AvgEngine::Utils::StringTools::Contains(line, "dance-single"))
						skipDiff = true;
					break;
				case 2: // diff author (most of the time... or its in the CREDIT tag if theres no diff author)
					// remove the last character
					workingDiff.Charter = trim.substr(0, trim.length() - 1);
					break;
				case 3:
					// diff name
					workingDiff.Name = trim.substr(0, trim.length() - 1);
					break;
				case 4:
					// diff rating
					std::string endTrim = trim.substr(0, trim.length() - 1);
					try
					{
						workingDiff.DifficultyRating = boost::lexical_cast<int>(endTrim);
					}
					catch (boost::bad_lexical_cast&)
					{
						AvgEngine::Logging::writeLog("[StepFile] [Warning] " + path + " on difficulty " + workingDiff.Name + " by " + workingDiff.Charter + " has a bad difficulty rating!");
						workingDiff.DifficultyRating = 0;
					}

					break;
				}
			}
			else
			{
				
				// We are in the notes section
				if (line.front() == ';' || line.front() == ',')
				{
					// Time for big brain music theory math

					if (measure.size() > 0)
					{
						float lengthInRows = 192.0f / static_cast<float>(measure.size());

						// Storage for the row index (and beat)
						int rowIndex = 0;
						float beat = 0;

						// Read the notes from the measure (for loop over the measure)
						for (int i = 0; i < measure.size(); i++)
						{
							float noteRow = (static_cast<float>(currentMeasure) * 192.0f) + (lengthInRows * static_cast<float>(rowIndex));

							beat = noteRow / 48.0f;

							for (int n = 0; n < measure[i].size(); n++) {
								Note note{};
								note.Row = noteRow;
								note.Beat = beat;
								note.Lane = n;
								if (measure[i][n] == '0')
									continue;
								switch (measure[i][n])
								{
								case '1':
									note.Type = NoteType_Tap;
									break;
								case '2':
									note.Type = NoteType_Head;
									break;
								case '3':
									note.Type = NoteType_Tail;
									break;
								case '4': // roll, but we convert it to a head.
									note.Type = NoteType_Head;
									break;
								case 'F':
									note.Type = NoteType_Fake;
									break;
								case 'M':
									note.Type = NoteType_Mine;
									break;
								}
								workingDiff.Notes.push_back(note);
							}

							rowIndex++;
						}

					}
					measure = {};
					currentMeasure++;
				}
				else
				{
					if (line.find("//") == std::string::npos)
						measure.push_back(line);
				}
			}

			// if line starts with ";", set state to 0
			if (line.front() == ';')
			{
				if (workingDiff.Notes.size() > 0)
					chartMetadata.Difficulties.push_back(workingDiff);
				workingDiff = {};
				*state = 0;
			}
		}

	public:

		StepFile(std::string _path) : ChartFile(_path)
		{
			chartMetadata.Chart_Type = 0;
		}

		void Parse() override
		{
			// load the file (from path), and check if it loaded correctly
			std::ifstream infile(path);
	
			if (!infile.good())
			{
				AvgEngine::Logging::writeLog("[StepFile] [Error] Couldn't load " + path + "!");
				return;
			}

			// Setup state variable,
			// 0 = header
			// 1 = bpms
			// 2 = stops
			// 3 = notes

			int state = 0;

			// Setup file variables
			std::string line = "";
			int lineNumber = 0;

			// Start reading the file
			while (std::getline(infile, line)) {
				if (line.empty())
				{
					lineNumber++;
					continue; // Ignore empty lines
				}
				switch (state)
				{
				case 0:
					skipDiff = false;
					if (line.starts_with("#NOTES"))
					{
						// Reset line number for this section
						lineNumber = 0;
						// Also measure stuff
						currentMeasure = 0;
						measure = {};
						// Switch the state to notes
						state = 3;
						break;
					}
					else if (line.starts_with("#BPMS"))
					{
						// Reset line number for this section
						lineNumber = 0;
						// Switch the state to bpms
						state = 1;
						// Parse the current line of bpms
						ParseBPMS(line, &state);
						break;
					}
					else if (line.starts_with("#STOPS"))
					{
						// Reset line number for this section
						lineNumber = 0;
						// Switch the state to stops
						state = 2;
						// Parse the current line of stops
						ParseStops(line, &state);
						break;
					}
					else // Header metadata
					{
						// Get key value pairs by seperating the line by ":", and removing the start character, and end character.
						std::string key = line.substr(1, line.find_first_of(":") - 1);
						std::string value = line.substr(line.find_first_of(":") + 1, line.length());
						value = value.substr(0, value.length() - 1);

						// Set the metadata (if statement bad, ooooo. haunt your dreams!!!)
						if (key == "TITLE")
							chartMetadata.Song_Title = value;
						else if (key == "SUBTITLE")
							chartMetadata.Song_Subtitle = value;
						else if (key == "ARTIST")
							chartMetadata.Song_Artist = value;
						else if (key == "CREDIT")
							chartMetadata.Song_Credit = value;
						else if (key == "BANNER")
							chartMetadata.Song_Banner = value;
						else if (key == "BACKGROUND")
							chartMetadata.Song_Background = value;
						else if (key == "MUSIC")
							chartMetadata.Song_File = value;
						else if (key == "OFFSET")
						{
							try
							{
								chartMetadata.Song_Offset = boost::lexical_cast<float>(value);
							}
							catch (boost::bad_lexical_cast&)
							{
								AvgEngine::Logging::writeLog("[StepFile] [Warning] " + path + " has a bad chart offset!");
								chartMetadata.Song_Offset = 0;
							}
						}
						else if (key == "SAMPLESTART")
						{
							try
							{
								chartMetadata.previewStart = boost::lexical_cast<float>(value);
							}
							catch (boost::bad_lexical_cast&)
							{
								AvgEngine::Logging::writeLog("[StepFile] [Warning] " + path + " has a bad sample start!");
								chartMetadata.previewStart = 0;
							}
						}
					}

					break;
				case 1:
					ParseBPMS(line, &state);
					break;
				case 2:
					ParseStops(line, &state);
					break;
				case 3:
					if (!skipDiff)
						ParseNotes(line, lineNumber, &state);
					else
					{
						// If theres a ";" that means we can stop skipping
						if (line.back() == ';')
							state = 0;
					}
					break;
				}
				lineNumber++;
			}
			chartMetadata.Chart_Type = 1;

			// Construct timing


			int tpIndex = 1;
			int spIndex = 0;
			if (chartMetadata.TimingPoints.size() > 1)
			{
				if (chartMetadata.Stops.size() > 1)
				{
					while (tpIndex < chartMetadata.TimingPoints.size() || spIndex < chartMetadata.Stops.size())
					{
						TimingPoint& prevTp = chartMetadata.TimingPoints[tpIndex - 1];

						if (tpIndex == chartMetadata.TimingPoints.size())
						{
							float cts = 0;
							while (spIndex < chartMetadata.Stops.size())
							{
								StopPoint& sp = chartMetadata.Stops[spIndex];
								sp.StartTimestamp = (prevTp.StartTimestamp + (sp.StartBeat - prevTp.StartBeat) / (prevTp.Bpm / 60)) + cts;
								cts += sp.Length;
								spIndex++;
							}
							continue;
						}

						if (spIndex == chartMetadata.Stops.size())
						{
							while (tpIndex < chartMetadata.TimingPoints.size())
							{
								TimingPoint& tp = chartMetadata.TimingPoints[tpIndex];
								prevTp.EndTimestamp += (prevTp.EndBeat - prevTp.StartBeat) / (prevTp.Bpm / 60);
								tp.StartTimestamp = prevTp.EndTimestamp;
								tp.EndTimestamp = tp.StartTimestamp;
								tpIndex++;
								if (tpIndex == chartMetadata.TimingPoints.size())
									tp.EndTimestamp = INT_MAX;
							}
							continue;
						}

						TimingPoint& tp = chartMetadata.TimingPoints[tpIndex];
						StopPoint& sp = chartMetadata.Stops[spIndex];

						prevTp.EndBeat = tp.StartBeat;

						if (sp.StartBeat < prevTp.EndBeat)
						{
							sp.StartTimestamp = prevTp.EndTimestamp + (sp.StartBeat - prevTp.StartBeat) / (prevTp.Bpm / 60);
							prevTp.EndTimestamp += sp.Length;
							spIndex++;
						}
						else
						{
							prevTp.EndTimestamp += (prevTp.EndBeat - prevTp.StartBeat) / (prevTp.Bpm / 60);
							tp.StartTimestamp = prevTp.EndTimestamp;
							tp.EndTimestamp = tp.StartTimestamp;
							tpIndex++;
						}

					}
				}
				else
				{
					while (tpIndex < chartMetadata.TimingPoints.size())
					{
						TimingPoint& prevTp = chartMetadata.TimingPoints[tpIndex - 1];
						TimingPoint& tp = chartMetadata.TimingPoints[tpIndex];
						prevTp.EndTimestamp += (prevTp.EndBeat - prevTp.StartBeat) / (prevTp.Bpm / 60);
						tp.StartTimestamp = prevTp.EndTimestamp;
						tp.EndTimestamp = tp.StartTimestamp;
						tpIndex++;
						if (tpIndex == chartMetadata.TimingPoints.size())
							tp.EndTimestamp = INT_MAX;
					}
				}
			}
			else
			{
				TimingPoint& prevTp = chartMetadata.TimingPoints[0];
				float cts = 0;
				while (spIndex < chartMetadata.Stops.size())
				{
					StopPoint& sp = chartMetadata.Stops[spIndex];
					sp.StartTimestamp = (prevTp.StartTimestamp + (sp.StartBeat - prevTp.StartBeat) / (prevTp.Bpm / 60)) + cts;
					cts += sp.Length;
					spIndex++;
				}
			}
			good = true;
		}
	};
}