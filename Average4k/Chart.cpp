#include "Chart.h"

std::vector<std::string> Chart::split(std::string str, char delimiter)
{
    std::vector < std::string > internal;
    std::stringstream ss(str);
    std::string tok;

    while (std::getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }

    return internal;
}

float Chart::getTimeFromBeat(float beat, bpmSegment seg) {
    float beatThing = (beat - seg.startBeat) / (seg.bpm / 60);
    return seg.startTime + (beatThing * 1000) - (meta.chartOffset * 1000);
}

float Chart::getTimeFromBeatOffset(float beat, bpmSegment seg) {
    float beatThing = (beat - seg.startBeat) / (seg.bpm / 60);
    return seg.startTime + (beatThing * 1000);
}

float Chart::getBeatFromTime(float timestamp, bpmSegment seg) {
    float result = seg.startBeat + ((((timestamp / 1000) - ((seg.startTime / 1000) - (meta.chartOffset))) * (seg.bpm / 60)));
    return result;
}

float Chart::getBeatFromTimeOffset(float timestamp, bpmSegment seg) {
    return seg.startBeat + ((((timestamp / 1000) - ((seg.startTime / 1000))) * (seg.bpm / 60)));
}

bpmSegment Chart::getSegmentFromTime(float time) {
    bpmSegment seg;
    seg.bpm = meta.bpms[0].bpm;
    seg.startBeat = 0;
    seg.startTime = 0;
    seg.endBeat = INT_MAX;
    seg.length = INT_MAX;

    for (int i = 0; i < meta.bpms.size(); i++) {
        bpmSegment segment = meta.bpms[i];
        if (time >= segment.startTime && time < (segment.startTime + segment.length))
            seg = segment;
    }


    return seg;
}

float Chart::getStopOffsetFromTime(float timestamp)
{
    float offset = 0;
    float beat = getBeatFromTime(timestamp,getSegmentFromTime(timestamp));
    for (int i = 0; i < meta.stops.size(); i++)
    {
        stopSegment& seg = meta.stops[i];
        if (seg.beat < beat)
            offset += seg.length;
    }
    return offset;
}


float Chart::getStopOffsetFromBeat(float beat)
{
    float offset = 0;
    for (int i = 0; i < meta.stops.size(); i++)
    {
        stopSegment& seg = meta.stops[i];
        if (seg.beat < beat)
            offset += seg.length;
    }
    return offset;
}

bpmSegment Chart::getSegmentFromBeat(float beat)
{
    bpmSegment seg;
    seg.bpm = meta.bpms[0].bpm;
    seg.startBeat = 0;
    seg.startTime = 0;
    seg.endBeat = INT_MAX;
    seg.length = INT_MAX;

    for (int i = 0; i < meta.bpms.size(); i++) {
        bpmSegment segment = meta.bpms[i];
        if (beat >= segment.startBeat && beat < segment.endBeat)
            seg = segment;
    }


    return seg;
}

void Chart::destroy() {
    // free memory
    if (!this)
        return;
}