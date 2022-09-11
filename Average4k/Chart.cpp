#include "Chart.h"

void Chart::getInfo()
{
	for (difficulty& d : meta.difficulties) {
		float measures = 0;

		float streamMeasures = 0;
		float jumpstreamMeasures = 0;
		float chordjackMeasures = 0;
		float handstreamMeasures = 0;
		float jackMeasures = 0;

		int curMeasure = 0;
		float lastBeat = -1;

		float jumps = 0;
		float chordJacks = 0;
		float hands = 0;
		float singles = 0;
		float quads = 0;
		float jacks = 0;

		float allNotes = 0;

		std::map < int, note > stuff;

		for (note n : d.notes) {
			bool huh = false;

			if (n.type == Note_Tail || n.type == Note_Mine)
				continue;

			if (stuff.size() > 0) {
				for (const auto& [k, v] : stuff) {
					if (v.beat >= n.beat - 0.01 && v.beat < n.beat + 0.01)
						huh = true;
					else {
						switch (stuff.size()) {
						case 1:
							if ((n.beat - v.beat) <= .5 && v.lane == n.lane)
							{
								singles--;
								jacks += 2;
							}
							else
								singles++;
							break;
						case 2:
							if ((n.beat - v.beat) <= .25 && v.lane == n.lane)
							{
								jumps -= 2;
								chordJacks += 2;
							}
							else
								jumps += 2;
							break;
						case 3:
							if ((n.beat - v.beat) <= .25 && v.lane == n.lane)
							{
								hands -= 3;
								chordJacks += 3;
							}
							else
								hands += 3;
							break;
						case 4:
							if ((n.beat - v.beat) <= .25 && v.lane == n.lane)
							{
								quads -= 4;
								chordJacks += 4;
							}
							else
								quads += 4;
							break;
						}
						stuff.clear();
						break;
					}

				}
			}

			stuff[n.lane] = n;

			if (lastBeat == -1)
				lastBeat = n.beat;

			if (lastBeat + 2 < n.beat && n.beat < lastBeat + 4) {
				curMeasure++;
				measures++;

				lastBeat = n.beat;

				float avgSingles = singles / allNotes;
				float avgJumps = jumps / allNotes;
				float avgHands = hands / allNotes;
				float avgQuads = quads / allNotes;
				float avgChordJacks = chordJacks / allNotes;
				float avgJacks = jacks / allNotes;

				if (avgSingles > 0.2 && (avgHands > 0.2 || avgQuads > 0.2) && jacks < 0.2)
					handstreamMeasures++;
				else if (avgJumps > 0 && avgSingles > 0 && chordJacks < 0.2)
					jumpstreamMeasures++;
				else if (avgJacks > chordJacks && avgSingles < 0.5)
					jackMeasures++;
				else if (chordJacks > 0)
					chordjackMeasures++;
				else if (avgSingles > 0)
					streamMeasures++;

				jumps = 0;
				hands = 0;
				singles = 0;
				quads = 0;
				jacks = 0;
				allNotes = 0;
			}
			else if (n.beat > lastBeat + 4)
			{
				curMeasure++;
				measures++;

				lastBeat = n.beat;
				jumps = 0;
				hands = 0;
				singles = 0;
				quads = 0;
				jacks = 0;
				allNotes = 0;
			}

			allNotes++;
		}

		info in;
		if (chordjackMeasures != 0)
			in.chordjack = chordjackMeasures / measures;
		if (handstreamMeasures != 0)
			in.handstream = handstreamMeasures / measures;
		if (streamMeasures != 0)
			in.stream = streamMeasures / measures;
		if (jackMeasures != 0)
			in.jacks = jackMeasures / measures;
		if (jumpstreamMeasures != 0)
			in.jumpstream = jumpstreamMeasures / measures;

		float sum = in.stream + in.jumpstream + in.chordjack + in.handstream + in.jacks;

		if (sum < 1)
		{
			if (in.stream > (in.jumpstream + in.chordjack + in.handstream + in.jacks))
				in.stream += (1 - sum);
			if (in.chordjack > (in.jumpstream + in.stream + in.handstream + in.jacks))
				in.chordjack += (1 - sum);
			if (in.handstream > (in.jumpstream + in.stream + in.chordjack + in.jacks))
				in.handstream += (1 - sum);
			if (in.jacks > (in.jumpstream + in.stream + in.chordjack + in.handstream))
				in.handstream += (1 - sum);
			if (in.jumpstream > (in.handstream + in.stream + in.chordjack + in.jacks))
				in.jumpstream += (1 - sum);
		}


		d.info = in;

	}
}

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
    return (seg.startTime + (beatThing * 1000) - (meta.chartOffset * 1000)) ;
}

float Chart::getTimeFromBeatOffset(float beat, bpmSegment seg) {
    float beatThing = (beat - seg.startBeat) / (seg.bpm / 60);
    return (seg.startTime + (beatThing * 1000));
}

float Chart::getBeatFromTime(float timestamp, bpmSegment seg) {
    float chartOffset = meta.chartOffset;
    float result = seg.startBeat + (((timestamp / 1000)) - ((seg.startTime / 1000) - chartOffset)) * (seg.bpm / 60);
    return result;
}

float Chart::getBeatFromTimeOffset(float timestamp, bpmSegment seg) {
    return seg.startBeat + ((((timestamp  / 1000) - (((seg.startTime)/ 1000))) * (seg.bpm / 60)));
}

bpmSegment Chart::getSegmentFromTime(float time) {
    float offsetTime = time + (meta.chartOffset * 1000);
    if (offsetTime >= previouslyFound.startTime && offsetTime < ((previouslyFound.startTime) + previouslyFound.length))
        return previouslyFound;
    bpmSegment seg;
    seg.bpm = meta.bpms[0].bpm;
    seg.startBeat = 0;
    seg.startTime = 0;
    seg.endBeat = INT_MAX;
    seg.length = INT_MAX;
    for (int i = 0; i < meta.bpms.size(); i++) {
        bpmSegment segment = meta.bpms[i];
        if (offsetTime >= segment.startTime && offsetTime < ((segment.startTime) + segment.length))
        {
            seg = segment;
            previouslyFound = segment;
            break;
        }
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
    if (beat >= previouslyFound.startBeat && beat < previouslyFound.endBeat)
        return previouslyFound;
    bpmSegment seg;
    seg.bpm = meta.bpms[0].bpm;
    seg.startBeat = 0;
    seg.startTime = 0;
    seg.endBeat = INT_MAX;
    seg.length = INT_MAX;

    for (int i = 0; i < meta.bpms.size(); i++) {
        bpmSegment segment = meta.bpms[i];
        if (beat >= segment.startBeat && beat < segment.endBeat)
        {
            seg = segment;
            previouslyFound = segment;
            break;
        }
    }


    return seg;
}

void Chart::destroy() {
    // free memory
    for (difficulty& d : meta.difficulties)
    {
        d.notes.clear();
    }
    meta.difficulties.clear();
}