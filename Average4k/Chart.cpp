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
    return seg.startTime + ((beat - seg.startBeat) / (seg.bpm / 60)) - meta.chartOffset;
}

float Chart::getTimeFromBeatOffset(float beat, bpmSegment seg) {
    return seg.startTime + ((beat - seg.startBeat) / (seg.bpm / 60));
}

float Chart::getBeatFromTime(float timestamp, bpmSegment seg) {
    return seg.startBeat + (((timestamp / 1000) - (seg.startTime - meta.chartOffset)) * (seg.bpm / 60));
}

float Chart::getBeatFromTimeOffset(float timestamp, bpmSegment seg) {
    return seg.startBeat + (((timestamp / 1000) - (seg.startTime)) * (seg.bpm / 60));
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
        if (time >= segment.startTime * 1000.f && time < (segment.startTime + segment.length) * 1000.f)
            seg = segment;
    }


    return seg;
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