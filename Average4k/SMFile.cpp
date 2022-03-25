#include "SMFile.h"

SMFile::SMFile(std::string path, std::string folder, bool doReplace = true) {

    if(path.find("\\") != std::string::npos)
        path.replace(path.find("\\"), sizeof("\\") - 1, "/");

    std::ifstream infile(path);

    auto pathSplit = Chart::split(path, '/');

    meta.folder = folder;

    std::string line;


    bool readingNotes = false;
    bool readingBPMS = false;
    bool readingSTOPS = false;
    int bpmIndex = 0;

    int diffIndex = 0;
    float beat = 0;
    int measureIndex = 0;

    note notes[4] = {};

    std::vector < std::string > measure;

    // I don't want to talk about this code >:(

    bool isSingled = false;

    bool skippingDiff = false;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string s = iss.str();

        bool cont = true;

        if (iss.str().find("//") != std::string::npos)
            cont = false;
        else if (iss.str().find("dance-single") != std::string::npos)
        {
            isSingled = true;
        }


        if (iss.str().find(",") != std::string::npos)
            cont = true;


        if (cont)
        {
            if (!readingNotes) {
                if (skippingDiff)
                {
                    if (iss.str().find(";") != std::string::npos)
                        skippingDiff = false;
                }
                else
                {
                    s.erase(std::remove(s.begin(), s.end(), ';'), s.end());
                    std::vector < std::string > stuff = Chart::split(s, ':');

                    if (readingBPMS)
                    {
                        if (iss.str() == ";" || iss.str()[0] == '#')
                            readingBPMS = false;
                        else
                        {
                            stuff[0].erase(std::remove(stuff[0].begin(), stuff[0].end(), ','), stuff[0].end());
                            std::vector < std::string > bpmSeg = Chart::split(stuff[0], '=');
                            bpmSegment seg;
                            seg.startBeat = std::stod(bpmSeg[0]);
                            seg.endBeat = INT_MAX;
                            seg.length = INT_MAX;
                            seg.bpm = std::stof(bpmSeg[1]);
                            seg.startTime = -(meta.chartOffset) * 1000;

                            if (bpmIndex != 0) // previous lol
                            {
                                bpmSegment& prevSeg = meta.bpms[bpmIndex - 1];
                                prevSeg.endBeat = seg.startBeat;
                                prevSeg.length = ((prevSeg.endBeat - prevSeg.startBeat) / (prevSeg.bpm / 60)) * 1000;
                                seg.startTime = prevSeg.startTime + prevSeg.length;
                            }

                            meta.bpms.push_back(seg);
                            bpmIndex++;
                        }
                    }
                    else if (readingSTOPS)
                    {
                        if (iss.str() == ";" || iss.str()[0] == '#')
                            readingSTOPS = false;
                        else
                        {
                            stuff[0].erase(std::remove(stuff[0].begin(), stuff[0].end(), ','), stuff[0].end());
                            std::vector < std::string > bpmSeg = Chart::split(stuff[0], '=');
                            stopSegment seg;
                            seg.beat = std::stod(bpmSeg[0]);
                            seg.length = std::stof(bpmSeg[1]) * 1000;

                            meta.stops.push_back(seg);

                        }
                    }
                    if (!readingBPMS && !readingSTOPS)
                    {

                        if (stuff.size() != 0)
                        {
                            if (stuff[0] == "#BPMS") {
                                // gather bpms
                                readingBPMS = true;
                                if (stuff.size() != 1)
                                {
                                    std::vector < std::string > bpmSeg = Chart::split(stuff[1], ',');
                                    if (bpmSeg.size() != 0)
                                    {
                                        for (int ii = 0; ii < bpmSeg.size(); ii += 2)
                                        {
                                            bpmSegment seg;
                                            seg.startBeat = std::stod(Chart::split(bpmSeg[ii], '=')[0]);
                                            seg.endBeat = INT_MAX;
                                            seg.length = INT_MAX;
                                            seg.bpm = std::stof(Chart::split(bpmSeg[ii], '=')[1]);
                                            seg.startTime = 0;

                                            if (bpmIndex != 0) // previous lol
                                            {
                                                bpmSegment& prevSeg = meta.bpms[bpmIndex - 1];
                                                prevSeg.endBeat = seg.startBeat;
                                                prevSeg.length = ((prevSeg.endBeat - prevSeg.startBeat) / (prevSeg.bpm / 60)) * 1000;
                                                seg.startTime = prevSeg.startTime + prevSeg.length;
                                            }

                                            meta.bpms.push_back(seg);
                                            bpmIndex++;
                                        }
                                    }
                                }
                            }

                            if (stuff[0] == "#STOPS") {
                                readingSTOPS = true;
                                if (stuff.size() != 1)
                                {
                                    std::vector < std::string > bpmSeg = Chart::split(stuff[1], ',');
                                    if (bpmSeg.size() != 0)
                                    {
                                        for (int ii = 0; ii < bpmSeg.size(); ii += 2)
                                        {
                                            stopSegment seg;
                                            seg.beat = std::stod(Chart::split(bpmSeg[ii], '=')[0]);
                                            seg.length = std::stof(Chart::split(bpmSeg[ii], '=')[1]) * 1000;

                                            meta.stops.push_back(seg);
                                        }
                                    }
                                }
                            }

                            if (stuff[0] == "#NOTES") {
                                if (isSingled)
                                {
                                    readingNotes = true;
                                    skippingDiff = false;
                                    difficulty diff;
                                    diff.charter = "n/a";
                                    diff.name = "n/a";
                                    meta.difficulties.push_back(diff);
                                }
                                else
                                    skippingDiff = true;
                                isSingled = false;
                            }
                            if (stuff.size() != 1)
                            {
                                if (stuff[0] == "#BACKGROUND")
                                    meta.background = stuff[1];
                                if (stuff[0] == "#BANNER")
                                    meta.banner = stuff[1];
                                if (stuff[0] == "#ARTIST")
                                    meta.artist = stuff[1];
                                if (stuff[0] == "#TITLE")
                                    meta.songName = stuff[1];
                                if (stuff[0] == "#MUSIC")
                                    meta.audio = stuff[1];
                                if (stuff[0] == "#OFFSET")
                                    meta.chartOffset = std::stof(stuff[1]);
                            }
                        }
                    }
                }
            }
            else {
                std::vector < std::string > stuff = Chart::split(s, ':');
                if (iss.str().find(":") != std::string::npos) {
                    stuff[0].erase(std::remove(stuff[0].begin(), stuff[0].end(), ':'), stuff[0].end());
                    stuff[0].erase(std::remove(stuff[0].begin(), stuff[0].end(), ' '), stuff[0].end());
                    switch (diffIndex) {
                    case 1:
                        meta.difficulties.back().charter = stuff[0];
                        break;
                    case 2:
                        meta.difficulties.back().name = stuff[0];
                        break;
                    }
                    diffIndex++;
                }
                else {
                    diffIndex = 0;

                    // NOTE STUFF!!!!!

                    if (iss.str().find(",") == std::string::npos && iss.str().find(";") == std::string::npos)
                        measure.push_back(iss.str());
                    else {
                        float lengthInRows = 192 / (measure.size());

                        int rowIndex = 0;

                        for (int i = 0; i < measure.size(); i++) {
                            float noteRow = (measureIndex * 192) + (lengthInRows * rowIndex);

                            beat = noteRow / 48;

                            for (int n = 0; n < measure[i].size(); n++) {
                                char thing = measure[i][n];

                                if (thing == '0')
                                    continue;

                                note note;
                                note.beat = beat;
                                note.lane = n;
                                if (thing != 'M')
                                {
                                    switch (thing)
                                    {
                                    case '1':
                                        note.type = Note_Normal;
                                        break;
                                    case '2':
                                        note.type = Note_Head;
                                        notes[n] = note;
                                        break;
                                    case '3':
                                        note.type = Note_Tail;
                                        note.connectedBeat = notes[n].beat;
                                        break;
                                    case '4':
                                        note.type = Note_Head;
                                        notes[n] = note;
                                        break;

                                    }
                                }
                                else
                                    note.type = Note_Mine;

                                meta.difficulties.back().notes.push_back(note);
                            }
                            rowIndex++;
                        }


                        measure = {};
                        measureIndex++;
                    }

                }

                if (iss.str().find(";") != std::string::npos) {
                    readingNotes = false;
                    measureIndex = 0;
                    beat = 0;
                    
                }
            }
        }
    }
    infile.close();
}