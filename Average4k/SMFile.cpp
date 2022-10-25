#include "SMFile.h"
#include "Game.h"
#include "Helpers.h"

SMFile::SMFile(std::string path, std::string folder, bool doReplace = true) {

    MUTATE_START

        if (path.find("\\") != std::string::npos)
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

    bool doubleChart = false;

    bool skippingDiff = false;

    int lineNumber = 0;

    std::string lines;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string s = iss.str();
        lineNumber++;
        bool cont = true;

        lines += s;

        if (iss.str().find("//") != std::string::npos)
        {
            cont = false;

            if (iss.str().find("--") != std::string::npos)
            {
                if (iss.str().find("dance-single") == std::string::npos)
                {
                    doubleChart = true;
                    //Game::asyncShowErrorWindow("Chart Parse Error (" + meta.songName + ")", "Line " + std::to_string(lineNumber) + " | Incorrect chart format", false);
                    std::cout << path << " Has a non supported chart format on Line " << lineNumber << std::endl;
                }
                else
                    doubleChart = false;
            }
        }

        if (doubleChart)
        {
            continue;
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
                            std::vector <std::string> uhuh = Chart::split(stuff[0], ',');
                            for (std::string s : uhuh)
                            {
                                if (s.size() == 0)
                                    continue;
                                std::vector < std::string > bpmSeg = Chart::split(s, '=');
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
                                        for (int ii = 0; ii < bpmSeg.size(); ii += 1)
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
                                    if (bpmSeg.size() > 1)
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
                                readingNotes = true;
                                skippingDiff = false;
                                difficulty diff;
                                diff.charter = "n/a";
                                diff.name = "n/a";
                                meta.difficulties.push_back(diff);

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
                                if (stuff[0] == "#SAMPLESTART")
                                    meta.start = std::stof(stuff[1]) * 1000;
                                if (stuff[0] == "#OFFSET")
                                    meta.chartOffset = std::stof(stuff[1]);
                                if (stuff[0] == "#CDTITLE")
                                    meta.cdtitle = stuff[1];
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
                        if (measure.size() == 0)
                        {
                            cont = true;
                            std::cout << path << " has no notes." << std::endl;
                            continue;
                        }
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
                                note.row = noteRow;
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

    if (meta.bpms.size() == 0)
    {
        dontUse = true;
    }

    if (meta.audio != "")
    {
        meta.ext = Chart::split(meta.audio, '.')[1];
        std::transform(meta.ext.begin(), meta.ext.end(), meta.ext.begin(), Helpers::asciitolower);
    }
    else
        dontUse = true;

    meta.hash = Helpers::setHash(lines);

    MUTATE_END
}

float getSmallestSnap(std::vector<note> notes)
{
    float smallestSnap = 1.0f;

    if (notes.size() == 0)
        return 1.0f;

    for (note n : notes)
    {
        float beatRow = (n.beat) * 48;

        float newSnap = 1.0f;

        if (fmod(beatRow, (192 / 4)) == 0)
            newSnap = 1.0f;
        else if (fmod(beatRow, (192 / 8)) == 0)
            newSnap = 2.0f;
        else if (fmod(beatRow, (192 / 12)) == 0)
            newSnap = 3.0f;
        else if (fmod(beatRow, (192 / 16)) == 0)
            newSnap = 4.0f;
        else if (fmod(beatRow, (192 / 24)) == 0)
            newSnap = 6.0f;
        else if (fmod(beatRow, (192 / 32)) == 0)
            newSnap = 8.0f;
        else if (fmod(beatRow, (192 / 48)) == 0)
            newSnap = 12.0f;
        else if (fmod(beatRow, (192 / 64)) == 0)
            newSnap = 16.0f;
        else
            newSnap = 48.0f;
        if (smallestSnap < newSnap)
            smallestSnap = newSnap;
    }


    return 1.0f / smallestSnap;
}

void SMFile::SaveSM(chartMeta meta, std::string outFile)
{
    std::ofstream ostream;

    ostream.open(outFile, std::fstream::out);

    if (ostream.is_open())
    {
        ostream << "#TITLE:" << meta.songName << ";" << std::endl;
        ostream << "#ARTIST:" << meta.artist << ";" << std::endl;
        ostream << "#MUSIC:" << meta.audio << ";" << std::endl;
        ostream << "#BANNER:" << meta.banner << ";" << std::endl;
        ostream << "#BACKGROUND:" << meta.background << ";" << std::endl;
        ostream << "#SAMPLESTART:" << std::to_string(meta.start) << ";" << std::endl;
        ostream << "#OFFSET:" << std::to_string(meta.chartOffset) << ";" << std::endl;
        ostream << "#BPMS:";
        int ind = 0;
        for (bpmSegment seg : meta.bpms)
        {
            if (ind != 0)
                ostream << ",";
            ostream << std::to_string(seg.startBeat) << "=" << std::to_string(seg.bpm);
            ind++;
        }
        ostream << ";" << std::endl;
        ind = 0;
        ostream << "#STOPS:";
        for (stopSegment seg : meta.stops)
        {
            if (ind != 0)
                ostream << ",";
            ostream << std::to_string(seg.beat) << "=" << std::to_string(seg.length) << ",";
            ind++;
        }
        ostream << ";" << std::endl;

        for (difficulty diff : meta.difficulties)
        {
            ostream << "//---------------dance-single - " << diff.charter << " ----------------" << std::endl;
            ostream << "#NOTES:" << std::endl;
            ostream << "     dance-single:" << std::endl;
            ostream << "     " << diff.charter << ":" << std::endl;
            ostream << "     " + diff.name << ":" << std::endl;
            ostream << "     1:" << std::endl;
            ostream << "     0,0,0,0,0:" << std::endl;
            float lastBeat = diff.notes[(diff.notes.size() - 1)].beat;

            float lastMeasure = lastBeat / 4;

            for (int i = 0; i <= lastMeasure; i++)
            {
                int startRow = i * 192;
                int lastRow = (i + 1) * 192 - 1;
                std::vector<note> measureNotes;

                float startBeat = startRow / 48.0f;

                for (note n : diff.notes)
                {
                    if (n.beat >= startBeat && n.beat < startBeat + 4)
                        measureNotes.push_back(n);
                }

                int rowSpacing = lround(getSmallestSnap(measureNotes) * 48);

                for (int row = startRow; row <= lastRow; row += rowSpacing)
                {
                    float rowBeat = row / 48.0f;

                    std::string directions[4] = { "0","0","0","0"};

                    for (note& topNote : measureNotes)
                    {
                        if (topNote.beat == rowBeat)
                        {
                            switch (topNote.type)
                            {
                            case Note_Normal:
                                directions[topNote.lane] = "1";
                                break;
                            case Note_Head:
                                directions[topNote.lane] = "2";
                                break;
                            case Note_Tail:
                                directions[topNote.lane] = "3";
                                break;
                            }
                        }
                    }
                    
                    for (int i = 0; i < 4; i++)
                        ostream << directions[i];
                    ostream << std::endl;
                }
                ostream << "," << std::endl;
            }
            ostream << ";" << std::endl;
        }
        ostream.close();
    }
}
