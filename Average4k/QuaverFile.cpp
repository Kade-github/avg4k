#include "QuaverFile.h"
#include "Helpers.h"
#include "SMFile.h"
QuaverFile::QuaverFile()
{
    // bruh
}


bool bpm_sort(bpmSegment const& lhs, bpmSegment const& rhs) {
    return lhs.startBeat < rhs.startBeat;
}

bool note_sort(note const& lhs, note const& rhs) {
    return lhs.beat < rhs.beat;
}

chartMeta QuaverFile::returnChart(std::string path)
{
        meta.folder = path;
    meta.chartType = 1;

    bool generatedBPMS = false;

    bool noteStarted = false;

    std::string lines;

    for (const auto& e : std::filesystem::directory_iterator(path))
    {
        if (!ends_with(e.path().string(), ".qua"))
            continue;
        bool notes = false;
        bool bpm = false;
        std::ifstream infile(e.path());
        std::string line;

        difficulty diff;
        diff.name = "unknown";
        diff.charter = "unknown";

        bpmSegment seg;

        seg.startTime = 0;
        seg.startBeat = 0;
        seg.endBeat = INT_MAX;
        seg.length = INT_MAX;

        int bpmIndex = 0;

        float firstBruh = 0; // offset

        currentWorkingNote = {};
        currentWorkingNote.beat = -1;

        bool conti = false;

        while (std::getline(infile, line)) {
            lines += line;
            if (conti)
                break;
            std::istringstream iss(line);
            std::string s = iss.str();
            std::vector<std::string> split = Chart::split(s, ':'); // property: value
            if (split.size() != 1)
                split[1].erase(0, 1); // remove the starting space
            if (bpm && !generatedBPMS)
            {
                if (split.size() != 2)
                {
                    if (seg.startTime < 0)
                    {
                        bpm = false;
                        generatedBPMS = true;
                        continue;
                    }
                    if (meta.bpms.size() != 0)
                    {
                        bpmSegment& prevSeg = meta.bpms.back();
                        float endBeat = getBeatFromTimeOffset(seg.startTime, prevSeg);
                        prevSeg.endBeat = endBeat;
                        prevSeg.length = ((prevSeg.endBeat - prevSeg.startBeat) / (prevSeg.bpm / 60)) * 1000;
                    }
                    meta.bpms.push_back(seg); // last seg
                    bpm = false;
                    generatedBPMS = true;

                    std::vector<int> remove = {};

                    int index = 0;
                    for (bpmSegment& seg : meta.bpms)
                    {
                        if (index != 0)
                            if (meta.bpms[index - 1].bpm == seg.bpm)
                            {
                                remove.push_back(index);
                            }
                        index++;
                    }

                    index = 0;
                    for (int i : remove)
                    {
                        meta.bpms.erase(meta.bpms.begin() + (i - index));
                        index++;
                    }
                    std::sort(meta.bpms.begin(), meta.bpms.end(), &bpm_sort);

                    index = 0;
                    for (bpmSegment& seg : meta.bpms) // final pass
                    {
                        seg.length = INT_MAX;
                        seg.endBeat = INT_MAX;
                        if (index != 0)
                        {
                            bpmSegment& prevSeg = meta.bpms[index - 1];
                            prevSeg.endBeat = seg.startBeat;
                            prevSeg.length = ((prevSeg.endBeat - prevSeg.startBeat) / (prevSeg.bpm / 60)) * 1000;
                        }
                        index++;
                    }
                }
                else
                {
                    if (s[0] == '-' && bpmIndex != 0)
                    {
                        float endBeat = getBeatFromTimeOffset(std::stod(split[1]), seg);
                        seg.endBeat = endBeat;
                        seg.length = ((seg.endBeat - seg.startBeat) / (seg.bpm / 60)) * 1000;
                        meta.bpms.push_back(seg);
                        bpmSegment storage = seg;
                        seg = storage; // create a copy in another variable lol
                        seg.bpm = 0;
                        seg.startTime = 0;
                        seg.startBeat = endBeat;
                        seg.endBeat = INT_MAX;
                        seg.length = INT_MAX;
                    }
                    if (split[0] == "- StartTime")
                    {
                        if (bpmIndex == 0)
                        {
                            firstBruh = std::stod(split[1]);
                            if (firstBruh < 0)
                                firstBruh = 0;
                            meta.chartOffset = 0;
                            meta.proprietaryChartFormatOffset = firstBruh / 1000;
                        }
                        seg.startBeat = 0;
                        seg.startTime = (std::stod(split[1]));
                        if (seg.startTime < 0)
                            seg.startTime = 0;
                        bpmIndex++;
                    }
                    if (split[0] == "- Bpm") // for weird charts
                    {
                        seg.bpm = std::stod(split[1]);
                        seg.startBeat = 0;
                        seg.startTime = 0;
                        seg.endBeat = INT_MAX;
                        seg.length = INT_MAX;
                        bpmIndex++;
                    }
                    else if (split[0] == "  Bpm")
                    {
                        seg.bpm = std::stod(split[1]);
                        if (seg.startTime != 0)
                            seg.startBeat = (seg.startTime / 1000) * (seg.bpm / 60); // we can guess this cuz its the first bpm segment, we dont have to worry about bpm changes.
                    }
                }
            }
            else if (generatedBPMS && bpm)
                continue;
            if (notes)
            {
                if (split.size() != 2)
                    notes = false;
                if (split[0] == "- StartTime")
                {
                    float time = std::stod(split[1]);
                    float beat = getBeatFromTimeOffset(time, getSegmentFromTime(time, meta.bpms));

                    if (currentWorkingNote.beat != -1)
                    {
                        diff.notes.push_back(currentWorkingNote);
                        currentWorkingNote = {};
                    }
                    currentWorkingNote.type = Note_Normal;
                    currentWorkingNote.beat = beat;
                }
                else if (split[0] == "- Lane") // wtf quaver, why are you so weird
                {
                    currentWorkingNote = {};
                    currentWorkingNote.beat = -1;

                }
                else if (split[0] == "- EndTime") // wtf quaver, why are you so weird
                {
                    currentWorkingNote = {};
                    currentWorkingNote.beat = -1;

                }
                else
                {
                    if (split[0] == "  Lane")
                    {
                        currentWorkingNote.lane = std::stod(split[1]) - 1;
                    }

                    if (split[0] == "  EndTime")
                    {
                        float time = std::stod(split[1]);
                        float beat = getBeatFromTimeOffset(time, getSegmentFromTime(time, meta.bpms));

                        currentWorkingNote.type = Note_Head;

                        note tail;
                        tail.beat = beat;
                        tail.lane = currentWorkingNote.lane;
                        tail.type = Note_Tail;
                        diff.notes.push_back(tail);
                    }
                }
            }
            else
            {
                // blah blah, string has to be a enum value aka we cant use a switch case
                if (!generatedBPMS)
                {
                    if (split[0] == "AudioFile")
                    {
                        std::string& audio = meta.audio;
                        audio = split[1];
                    }
                    if (split[0] == "Title")
                    {
                        std::string& name = meta.songName;
                        name = split[1];
                    }
                }
                if (split[0] == "BackgroundFile")
                {
                    meta.background = split[1];
                    meta.banner = split[1];
                }
                if (split[0] == "SongPreviewTime")
                {
                    meta.start = std::stof(split[1]);
                }
                if (split[0] == "Artist")
                    meta.artist = split[1];
                if (split[0] == "Creator")
                    diff.charter = split[1];
                if (split[0] == "DifficultyName")
                    diff.name = split[1];
                if (split[0] == "Mode" && split[1] != "Keys4")
                    conti = true;
                if (split[0] == "TimingPoints")
                    bpm = true;
                if (split[0] == "HitObjects")
                    notes = true;
            }
        }

        if (diff.notes.size() != 0)
            meta.difficulties.push_back(diff);
        infile.close();
    }
    for (difficulty& diff : meta.difficulties)
    {
        std::sort(diff.notes.begin(), diff.notes.end(), &note_sort);
    }

    if (meta.audio.size() != 0)
    {

        meta.ext = Chart::split(meta.audio, '.')[1];
        std::transform(meta.ext.begin(), meta.ext.end(), meta.ext.begin(), Helpers::asciitolower);

        //SMFile::SaveSM(meta, path + "/converted.sm", meta.chartOffset);


        return meta;
    }
    else
        return {};
}