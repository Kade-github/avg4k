#include "QuaverFile.h"
#include "Helpers.h"
#include "yaml-cpp/yaml.h"



QuaverFile::QuaverFile()
{
   // bruh
}

struct TimingPoint {
    float StartTime;
    float Bpm;

    inline bool operator==(TimingPoint a) {
        if (a.StartTime == StartTime)
            return true;
        else
            return false;
    }
};

struct HitObject {
    float StartTime;
    int Lane;
    int EndTime;

    inline bool operator==(HitObject a) {
        if (a.StartTime == StartTime)
            return true;
        else
            return false;
    }
};

namespace YAML {

    template<>
    struct convert<TimingPoint> {
        static bool decode(const Node& node, TimingPoint& rhs) {
            if (!node.IsMap())
                return false;
            if (node["StartTime"])
            {
                rhs.StartTime = node["StartTime"].as<float>();
                rhs.Bpm = node["Bpm"].as<float>();
                return true;
            }
            if (node["Bpm"])
            {
                rhs.StartTime = 0;
                rhs.Bpm = std::stof(node["Bpm"].as<std::string>());
                return true;
            }
            return false;
        }
    };
    template<>
    struct convert<HitObject> {
        static bool decode(const Node& node, HitObject& rhs) {
            if (!node.IsMap())
                return false;
            if (!node["StartTime"])
            {
                if (!node["Lane"])
                    return false;
                else
                    rhs.StartTime = 0;
            }
            else
                rhs.StartTime = node["StartTime"].as<float>();
            rhs.Lane = node["Lane"].as<int>();
            auto eT = node["Endtime"];
            if (eT)
                rhs.EndTime = eT.as<int>();
            else
                rhs.EndTime = -1;
            return true;
        }
    };
}

bool note_sort(note const& lhs, note const& rhs) {
    return lhs.beat < rhs.beat;
}

chartMeta QuaverFile::returnChart(std::string path)
{
    MUTATE_START
    meta.folder = path;
    meta.chartType = 1;

    std::string lines;

    auto saved = std::filesystem::directory_iterator(path);

    for (const auto& e : saved)
    {
        std::string p = e.path().string();
        if (!ends_with(p, ".qua"))
            continue;
        YAML::Node qua = YAML::LoadFile(p);

        if (qua["Mode"].as<std::string>() != "Keys4")
            continue;

        difficulty diff;

        if (meta.audio.size() == 0)
        {
            meta.artist = qua["Artist"].as<std::string>();
            meta.audio = qua["AudioFile"].as<std::string>();
            meta.background = qua["BackgroundFile"].as<std::string>();
            meta.songName = qua["Title"].as<std::string>();
            std::vector<TimingPoint> points = qua["TimingPoints"].as<std::vector<TimingPoint>>();
            for (TimingPoint p : points)
            {
                bpmSegment seg;
                seg.bpm = p.Bpm;
                seg.length = INT_MAX;
                seg.endBeat = INT_MAX;
                if (meta.bpms.size() != 0)
                {
                    seg.startTime = p.StartTime;
                    seg.startBeat = (seg.startTime / 1000) * (seg.bpm / 60);
                }
                else
                {
                    seg.startTime = 0;
                    seg.startBeat = 0;
                }
                if (meta.bpms.size() != 0)
                {
                    bpmSegment& prevSeg = meta.bpms.back();
                    float endBeat = getBeatFromTimeOffset(seg.startBeat, seg);
                    prevSeg.endBeat = seg.startBeat;
                    prevSeg.length = ((prevSeg.endBeat - prevSeg.startBeat) / (prevSeg.bpm / 60)) * 1000;
                }
                meta.bpms.push_back(seg);
            }
        }

        diff.name = qua["DifficultyName"].as<std::string>();
        diff.charter = qua["Creator"].as<std::string>();

        std::vector<HitObject> hitObjects = qua["HitObjects"].as<std::vector<HitObject>>();

        meta.chartOffset = meta.bpms[0].startTime / 1000;
        meta.bpms[0].startTime = 0;
        diff.notes = {};
        for (HitObject obj : hitObjects)
        {
            note n;
            if (obj.EndTime != -1)
                n.type = Note_Head;
            else
                n.type = Note_Normal;
            n.beat = getBeatFromTimeOffset(obj.StartTime, getSegmentFromTime(obj.StartTime, meta.bpms));
            n.lane = obj.Lane - 1;

            diff.notes.push_back(n);
            if (n.type == Note_Head)
            {
                note tN;
                tN.type = Note_Tail;
                tN.beat = getBeatFromTimeOffset(obj.EndTime, getSegmentFromTime(obj.EndTime, meta.bpms));
                tN.lane = obj.Lane - 1;

                diff.notes.push_back(tN);
            }

            lines += std::to_string(n.beat) + std::to_string(n.lane) + std::to_string(n.type);
        }
        meta.difficulties.push_back(diff);
        remove(p.c_str());
    }
    for (difficulty& diff : meta.difficulties)
    {
        std::sort(diff.notes.begin(), diff.notes.end(), &note_sort);
    }

    meta.ext = Chart::split(meta.audio, '.')[1];
    std::transform(meta.ext.begin(), meta.ext.end(), meta.ext.begin(), Helpers::asciitolower);
    
    meta.hash = Helpers::setHash(lines);

    MUTATE_END
    return meta;
}
