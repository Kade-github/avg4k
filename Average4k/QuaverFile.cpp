#include "QuaverFile.h"
#include "Helpers.h"
#include "ryml/ryml.hpp"


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
