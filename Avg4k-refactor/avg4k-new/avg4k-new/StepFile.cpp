#include "StepFile.h"

void Average4k::Chart::StepFile::Parse()
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
						{
							tp.EndTimestamp = INT_MAX;
							tp.EndBeat = INT_MAX;
						}
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
				prevTp.EndBeat = tp.StartBeat;
				prevTp.EndTimestamp = prevTp.StartTimestamp + (prevTp.EndBeat - prevTp.StartBeat) / (prevTp.Bpm / 60);
				tp.StartTimestamp = prevTp.EndTimestamp;
				tpIndex++;
				if (tpIndex == chartMetadata.TimingPoints.size())
				{
					tp.EndTimestamp = INT_MAX;
					tp.EndBeat = INT_MAX;
				}
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
		prevTp.EndTimestamp = INT_MAX;
		prevTp.EndBeat = INT_MAX;
	}
	good = true;
}