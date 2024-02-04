/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "StepFile.h"
#include <AvgEngine/Utils/StringTools.h>

using namespace Average4k::Data::Chart::Providers;

void StepFile::Parse(std::string _path, bool metadataOnly)
{
	stream = std::ifstream(_path);

	if (!stream.good())
	{
		AvgEngine::Logging::writeLog("[Error] Couldn't load chart " + _path + "!");
		return;
	}

	path = _path;

	ParseMetadata(metadataOnly);

	isValid = true;
}

void StepFile::ParseMetadata(bool only)
{
	int state = 0;

	std::string line;

	int lineNumber = 0;

	while (std::getline(stream, line))
	{
		if (line.empty())
			continue;

		std::string key;
		std::string value;

		switch (state)
		{
		case 0:
			if (!only)
			{
				if (line.starts_with("#NOTES"))
				{
					lineNumber = 0;
					workingDiff = {};
					measure = {};
					currentMeasure = 0;
					state = 3;
					break;
				}
				else if (line.starts_with("#BPMS"))
				{
					state = 1;
					break;
				}
				else if (line.starts_with("#STOPS"))
				{
					state = 2;
					break;
				}
			}
			else
			{
				if (line.starts_with("#NOTES"))
					return;
			}

			key = line.substr(1, line.find_first_of(":") - 1);
			value = line.substr(line.find_first_of(":") + 1, line.length());

			value = value.substr(0, value.length() - 1);

			if (key == "TITLE")
				metadata.title = value;
			if (key == "SUBTITLE")
				metadata.subtitle = value;
			if (key == "ARTIST")
				metadata.artist = value;
			if (key == "GENRE")
				metadata.artist = value;
			if (key == "CREDIT")
				metadata.credit = value;
			if (key == "BANNER")
				metadata.banner = value;
			if (key == "BACKGROUND")
				metadata.background = value;
			if (key == "MUSIC")
				metadata.file = value;

			if (key == "OFFSET")
				metadata.offset = std::stof(value);

			if (key == "SAMPLESTART")
				metadata.previewStart = std::stof(value);
			break;
		case 1:
			ParseBPMS(line);

			if (line.find(';') != std::string::npos) // if the line ends with a semicolon, we're done
				state = 0;
			break;
		case 2:
			ParseStops(line);

			if (line.find(';') != std::string::npos) // if the line ends with a semicolon, we're done
				state = 0;
			break;
		case 3:
			ParseNotes(line, lineNumber);
			lineNumber++;

			if (line.front() == ';')
			{
				state = 0;
				lineNumber = 0;
			}
			break;
		}
	}
}

void StepFile::ParseBPMS(std::string line)
{
	std::string l = line;

	// Clean up the line
	if (l.find(':') != std::string::npos) // remove the starting "#BPMS:" if it exists
		l = l.substr(l.find(':') + 1);

	if (l.find(';') != std::string::npos) // remove the trailing semicolon if it exists
		l = l.substr(0, l.find(';'));

	// Split the line into individual BPMs
	std::vector<std::string> split = AvgEngine::Utils::StringTools::Split(l, ",");

	for (std::string s : split)
	{
		std::vector<std::string> bpm = AvgEngine::Utils::StringTools::Split(s, "=");

		if (bpm.size() != 2)
			continue;

		TimingPoint p;
		p.startBeat = std::stof(bpm[0]);
		p.bpm = std::stof(bpm[1]);

		if (timingPoints.size() == 0)
			p.startTimestamp = -metadata.offset;

		timingPoints.push_back(p);
	}

}

void StepFile::ParseStops(std::string line)
{
	std::string l = line;

	// Clean up the line
	if (l.find(':') != std::string::npos) // remove the starting "#STOPS:" if it exists
		l = l.substr(l.find(':') + 1);

	if (l.find(';') != std::string::npos) // remove the trailing semicolon if it exists
		l = l.substr(0, l.find(';'));

	// Split the line into individual BPMs
	std::vector<std::string> split = AvgEngine::Utils::StringTools::Split(l, ",");

	for (std::string s : split)
	{
		std::vector<std::string> stop = AvgEngine::Utils::StringTools::Split(s, "=");

		if (stop.size() != 2)
			continue;

		StopPoint p;
		p.startBeat = std::stof(stop[0]);
		p.length = std::stof(stop[1]);

		stopPoints.push_back(p);
	}
}

void StepFile::ParseNotes(std::string line, int& lineNumber)
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
			workingDiff.charter = trim.substr(0, trim.length() - 1);
			break;
		case 3:
			// diff name
			workingDiff.name = trim.substr(0, trim.length() - 1);
			break;
		case 4:
			// diff rating
			std::string endTrim = trim.substr(0, trim.length() - 1);
			try
			{
				workingDiff.rating = std::stoi(endTrim);
			}
			catch (std::exception e)
			{
				AvgEngine::Logging::writeLog("[StepFile] [Warning] " + metadata.title + " by " + metadata.artist + ", diff " + workingDiff.name + " charted by " + workingDiff.charter + " has a bad rating.");
				workingDiff.rating = 0;
			}
			break;
		}
	}
	else
	{
		if (!skipDiff)
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
							note.beat = beat;
							note.lane = n;
							if (measure[i][n] == '0')
								continue;
							switch (measure[i][n])
							{
							case '1':
								note.type = NoteType::Tap;
								break;
							case '2':
								note.type = NoteType::Head;
								break;
							case '3':
								note.type = NoteType::Tail;
								break;
							case '4': // roll, but we convert it to a head.
								note.type = NoteType::Head;
								break;
							case 'F':
								note.type = NoteType::Fake;
								break;
							case 'M':
								note.type = NoteType::Mine;
								break;
							}
							workingDiff.notes.push_back(note);
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
	}

	if (line.front() == ';')
	{
		if (workingDiff.notes.size() > 0)
			difficulties.push_back(workingDiff);
		workingDiff = {};
		skipDiff = false;
	}
}
