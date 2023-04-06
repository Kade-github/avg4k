#pragma once
#include "ChartFile.h"
namespace Average4k::Chart
{
	class StepFile : public ChartFile {
		
		// Storage variable for the current working difficulty.
		Difficulty workingDiff;

		void ParseStops(std::string& line, int* state)
		{
			// if line ends with ";", set state to 0
			if (line.back() == ';')
				*state = 0;
		}

		void ParseBPMS(std::string& line, int* state)
		{
			// if line ends with ";", set state to 0
			if (line.back() == ';')
				*state = 0;
		}

		void ParseNotes(std::string& line, int lineNumber, int* state)
		{
			// if line ends with ";", set state to 0
			if (line.back() == ';')
				*state = 0;
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
					if (line.starts_with("#NOTES"))
					{
						// Reset line number for this section
						lineNumber = 0;
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
						std::string key = line.substr(1, line.find(":") - 1);
						std::string value = line.substr(line.find(":") + 1, line.length());
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
						else if (key == "OFFSET" && AvgEngine::Utils::StringTools::isNumber(value))
							chartMetadata.Song_Offset = std::stof(value);
						else if (key == "SAMPLESTART" && AvgEngine::Utils::StringTools::isNumber(value))
							chartMetadata.previewStart = std::stof(value);
					}

					break;
				case 1:
					ParseBPMS(line, &state);
					break;
				case 2:
					ParseStops(line, &state);
					break;
				case 3:
					ParseNotes(line, lineNumber, &state);
					break;
				}
				lineNumber++;
			}
			good = true;
		}
	};
}