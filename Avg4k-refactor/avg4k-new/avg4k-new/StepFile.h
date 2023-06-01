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

		void Parse() override;
	};
}