/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "StepFile.h"
#include <AvgEngine/Utils/StringTools.h>

using namespace Average4k::Data::Chart::Providers;

void StepFile::Parse(std::wstring _path, bool metadataOnly)
{
	std::wifstream* stream = new std::wifstream(_path);

	std::string converted_str = AvgEngine::Utils::StringTools::Ws2s(_path);

	if (!stream->good())
	{
		AvgEngine::Logging::writeLog("[Error] Couldn't load chart " + converted_str + "!");
		return;
	}

	path = _path;

	ParseMetadata(metadataOnly, stream);

	isValid = true;

	stream->close();

	delete stream;
}

void StepFile::ParseMetadata(bool only, std::wifstream* stream)
{
	int state = 0;

	std::wstring line;

	int lineNumber = 0;

	bool skip = false;

	while (std::getline(*stream, line))
	{
		if (line.empty() || skip)
			continue;

		std::wstring key;
		std::wstring keyUnicode;
		std::wstring value;
		bool isTitle;

		switch (state)
		{
		case 0:
			if (!only)
			{
				if (line.starts_with(L"#NOTES"))
				{
					lineNumber = 0;
					workingDiff = {};
					measure = {};
					currentMeasure = 0;
					state = 3;
					break;
				}
				else if (line.starts_with(L"#BPMS"))
				{
					state = 1;
					ParseBPMS(line);

					if (line.find(';') != std::string::npos) // if the line ends with a semicolon, we're done
						state = 0;
					break;
				}
				else if (line.starts_with(L"#STOPS"))
				{
					state = 2;
					ParseStops(line);

					if (line.find(';') != std::string::npos) // if the line ends with a semicolon, we're done
						state = 0;
					break;
				}
			}
			else
			{
				if (line.starts_with(L"#NOTES"))
				{
					return;
				}
			}

			key = line.substr(line.find_first_of(L'#') + 1, line.find_first_of(L":"));

			if (key.find(L':') != std::string::npos)
				key = key.substr(0, key.find(L':'));

			value = line.substr(line.find_first_of(L':') + 1, line.length());

			value = value.substr(0, value.length() - 1);

			if (key == L"TITLE")
				metadata.title = value;
			if (key == L"TITLETRANSLIT")
				metadata.titleTranslit = value;
			if (key == L"SUBTITLE")
				metadata.subtitle = value;
			if (key == L"SUBTITLETRANSLIT")
				metadata.subtitleTranslit = value;
			if (key == L"ARTIST")
				metadata.artist = value;
			if (key == L"ARTISTTRANSLIT")
				metadata.artistTranslit = value;
			if (key == L"GENRE")
				metadata.genre = value;
			if (key == L"CREDIT")
				metadata.credit = value;
			if (key == L"BANNER")
				metadata.banner = value;
			if (key == L"BACKGROUND")
				metadata.background = value;
			if (key == L"MUSIC")
				metadata.file = value;

			if (key == L"OFFSET" && !value.empty())
				metadata.offset = std::stof(value);

			if (key == L"SAMPLESTART" && !value.empty())
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

StepFile::StepFile()
{

}

void StepFile::ParseBPMS(std::wstring line)
{
	std::wstring l = line;

	// Clean up the line
	if (l.find(L':') != std::string::npos) // remove the starting "#BPMS:" if it exists
		l = l.substr(l.find(L':') + 1);

	if (l.find(L';') != std::string::npos) // remove the trailing semicolon if it exists
		l = l.substr(0, l.find(L';'));

	// Split the line into individual BPMs
	std::vector<std::wstring> split = AvgEngine::Utils::StringTools::Split(l, L",");

	for (std::wstring s : split)
	{
		std::vector<std::wstring> bpm = AvgEngine::Utils::StringTools::Split(s, L"=");

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

void StepFile::ParseStops(std::wstring line)
{
	std::wstring l = line;

	// Clean up the line
	if (l.find(':') != std::string::npos) // remove the starting "#STOPS:" if it exists
		l = l.substr(l.find(':') + 1);

	if (l.find(';') != std::string::npos) // remove the trailing semicolon if it exists
		l = l.substr(0, l.find(';'));

	// Split the line into individual BPMs
	std::vector<std::wstring> split = AvgEngine::Utils::StringTools::Split(l, L",");

	for (std::wstring s : split)
	{
		std::vector<std::wstring> stop = AvgEngine::Utils::StringTools::Split(s, L"=");

		if (stop.size() != 2)
			continue;

		StopPoint p;
		p.startBeat = std::stof(stop[0]);
		p.length = std::stof(stop[1]);

		stopPoints.push_back(p);
	}
}

void StepFile::ParseNotes(std::wstring line, int& lineNumber)
{
	// if it contains a ":" we are in a metadata section of the notes
	if (line.find(':') != std::string::npos)
	{
		std::wstring trim = AvgEngine::Utils::StringTools::Trim(line);
		switch (lineNumber)
		{
		case 0: // chart style
			// if its not a dance single, we dont fuck wit it.
			if (!AvgEngine::Utils::StringTools::Contains(line, L"dance-single"))
				skipDiff = true;
			break;
		case 1: // diff author (most of the time... or its in the CREDIT tag if theres no diff author)
			// remove the last character
			workingDiff.charter = trim.substr(0, trim.length() - 1);
			break;
		case 2:
			// diff name
			workingDiff.name = trim.substr(0, trim.length() - 1);
			break;
		case 3:
			// diff rating
			std::wstring endTrim = trim.substr(0, trim.length() - 1);
			try
			{
				workingDiff.rating = std::stoi(endTrim);
			}
			catch (std::exception e)
			{
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
				if (line.find(L"//") == std::wstring::npos)
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
