#include "OsuFile.h"
#include "Helpers.h"
bool osuCompareNoteByBeat(const note a, const note b)
{
	return a.beat < b.beat;
}


OsuFile::OsuHeaderType OsuFile::getHeaderFromString(std::string s)
{
	if (s == "General")
		return (OsuFile::OsuHeaderType)0;
	if (s == "Editor")
		return (OsuFile::OsuHeaderType)1;
	if (s == "Metadata")
		return (OsuFile::OsuHeaderType)2;
	if (s == "Difficulty")
		return (OsuFile::OsuHeaderType)3;
	if (s == "Events")
		return (OsuFile::OsuHeaderType)4;
	if (s == "TimingPoints")
		return (OsuFile::OsuHeaderType)5;
	if (s == "HitObjects")
		return (OsuFile::OsuHeaderType)6;
}

OsuFile::OsuFile(std::string path)
{
	if (path.find("\\") != std::string::npos)
		path.replace(path.find("\\"), sizeof("\\") - 1, "/");

	std::vector<difficulty> diffs;

	std::string lines;

	for (auto& e : std::filesystem::directory_iterator(path))
	{
		if (ends_with(e.path().string(), ".osu"))
		{
			difficulty diff = generateDiff(e.path().string(), (meta.bpms.size() == 0 ? &meta : NULL), lines);
			if (diff.name == "Not mania" && diff.notes.size() == 0)
			{
				Logging::writeLog(e.path().string() + " is not a mania chart!");
				continue;
			}
			if (diff.name == "Not a 4k chart" && diff.notes.size() == 0)
			{
				Logging::writeLog(e.path().string() + " is not a 4k chart!");
				continue;
			}

			diffs.push_back(diff);
		}
	}
	meta.chartType = 2;
	meta.folder = path;
	meta.difficulties = diffs;

	meta.hash = Helpers::setHash(lines);

	meta.ext = Chart::split(meta.audio, '.')[1];
	std::transform(meta.ext.begin(), meta.ext.end(), meta.ext.begin(), Helpers::asciitolower);
}

difficulty OsuFile::generateDiff(std::string file, chartMeta* toAdd, std::string& lines)
{
	std::ifstream infile(file);
	std::string line;

	int osuVersion = 0;

	OsuHeaderType currentHeader = OSUHEADER_None;

	bool shouldAddToMeta = toAdd != NULL;

	int bpmIndex = 0;

	difficulty diff;
	while (std::getline(infile, line)) {
		lines += line;
		if (line.size() == 0)
			continue;
		if (line.starts_with("/"))
			continue;
		if (line.starts_with("osu file format v"))
			osuVersion = std::stoi(Chart::split(line, 'v')[1]);

		if (line.starts_with("["))
		{
			std::string header = Chart::split(line, '[')[1];
			currentHeader = getHeaderFromString(header.substr(0,header.size() - 1));
			continue;
		}

		std::string key;
		std::string value;

		// these have weird ass values that aren't key:value pairs

		if (currentHeader != OSUHEADER_None 
			&& currentHeader != OSUHEADER_TimingPoints
			&& currentHeader != OSUHEADER_Events
			&& currentHeader != OSUHEADER_HitObjects)
		{
			std::vector<std::string> keyValue = Chart::split(line, ':');
			if (keyValue.size() > 0)
				key = keyValue[0];
			if (keyValue.size() > 1)
			{
				value = keyValue[1];
				if (value.size() != 0)
					if (value[0] == ' ') // remove leading space
						value = value.substr(1, value.size());
			}
		}

		switch (currentHeader)
		{
		case OSUHEADER_General:
			if (shouldAddToMeta)
			{
				if (key == "AudioFilename")
					toAdd->audio = value;
				if (key == "PreviewTime")
				{
					float start = std::stof(value);
					if (start != -1)
						toAdd->start = start;
				}
				if (key == "Mode")
				{
					int mode = std::stoi(value);
					if (mode != 3)
					{
						diff.name = "Not mania";
						return diff;
					}
				}
			}
			break;
		case OSUHEADER_Metadata:
			if (shouldAddToMeta)
			{
				if (key == "Title")
					toAdd->songName = value;
				if (key == "Artist")
					toAdd->artist = value;
			}
			if (key == "Creator")
				diff.charter = value;
			if (key == "Version")
				diff.name = value;
			break;
		case OSUHEADER_Difficulty:
			if (key == "CircleSize")
			{
				if (value != "4")
				{
					diff.name = "Not a 4k chart";
					return diff;
				}
			}
			break;
		case OSUHEADER_Events:
			//  0         1         2
			//  eventType,startTime,eventParams
			//  https://osu.ppy.sh/wiki/en/Client/File_formats/Osu_%28file_format%29#events
			if (shouldAddToMeta)
			{
				if (toAdd->background.size() == 0)
				{
					std::vector<std::string> eventData = Chart::split(line, ',');

					if (eventData[0] == "0")
					{
						// and also remove the two "" around it
						toAdd->background = eventData[2].substr(1,eventData[2].size() - 2);
						toAdd->banner = toAdd->background;
					}
				}
			}
			break;
		case OSUHEADER_TimingPoints:
			// 0    1          2     3         4           5      6           7
			// time,beatLength,meter,sampleSet,sampleIndex,volume,uninherited,effects
			// syntax from https://osu.ppy.sh/wiki/en/Client/File_formats/Osu_%28file_format%29#timing-points

			if (shouldAddToMeta)
			{
				std::vector<std::string> timingpointData = Chart::split(line, ',');

				if (timingpointData[6] == "0") // its inherited, meaning its a sv. so ignore
					continue;

				bpmSegment seg;
				if (bpmIndex != 0)
					seg.startBeat = getBeatFromTime(std::stof(timingpointData[0]), meta.bpms[bpmIndex - 1]);
				else
				{
					seg.startBeat = 0;
					toAdd->chartOffset = std::stof(timingpointData[0]);
				}

				seg.endBeat = INT_MAX;
				seg.length = INT_MAX;
				seg.bpm = (double)(1 / std::stod(timingpointData[1]) * 1000 * 60);

				if (bpmIndex != 0)
					seg.startTime = std::stof(timingpointData[0]);
				else
					seg.startTime = 0;

				if (bpmIndex != 0) // previous lol
				{
					bpmSegment& prevSeg = meta.bpms[bpmIndex - 1];
					prevSeg.endBeat = seg.startBeat;
					prevSeg.length = ((prevSeg.endBeat - prevSeg.startBeat) / (prevSeg.bpm / 60)) * 1000;
				}

				meta.bpms.push_back(seg);
				bpmIndex++;
			}
			break;
		case OSUHEADER_HitObjects:
			//                    0 1 2    3    4        5            6
			// defualt hitObjects x,y,time,type,hitSound,objectParams,hitSample
			//                            0 1 2    3    4        5       6
			// default mania hold objects x,y,time,type,hitSound,endTime:hitSample
			// https://osu.ppy.sh/wiki/en/Client/File_formats/Osu_%28file_format%29#hit-objects
			
			std::vector<std::string> hitObjectData = Chart::split(line, ',');

			// this is the most fuckiest shit i've ever seen
			// thanks ppy!
			int col = std::floor(std::stoi(hitObjectData[0]) * 4 / 512);


			bpmSegment seg = getSegmentFromTime(std::stof(hitObjectData[2]));

			note n;
			n.lane = col;
			n.beat = getBeatFromTime(std::stof(hitObjectData[2]), seg);

			if (hitObjectData[3] == "128") // hold
			{
				note second;

				bpmSegment segSec = getSegmentFromTime(std::stof(hitObjectData[5]));

				second.beat = getBeatFromTime(std::stof(hitObjectData[5]), segSec);
				second.type = Note_Tail;
				second.connectedBeat = n.beat;
				second.lane = col;
				n.type = Note_Head;

				diff.notes.push_back(n);
				diff.notes.push_back(second);

			}
			else
			{
				n.type = Note_Normal;
				diff.notes.push_back(n);
			}

			break;
		}
	}
	std::sort(diff.notes.begin(), diff.notes.end(), osuCompareNoteByBeat);
	return diff;
}
