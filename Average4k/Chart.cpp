#include "Chart.h"
#include <numeric>

void Chart::getInfo()
{
	for (difficulty& d : meta.difficulties) {
		float measures = 0;

		float streamMeasures = 0;
		float jumpstreamMeasures = 0;
		float chordjackMeasures = 0;
		float handstreamMeasures = 0;
		float jackMeasures = 0;
		float technicalMeasures = 0;

		int curMeasure = 0;
		float lastBeat = -1;

		float jumps = 0;
		float chordJacks = 0;
		float hands = 0;
		float singles = 0;
		float quads = 0;
		float jacks = 0;
		float gluts = 0;

		float allNotes = 0;

		float differentRowNotes = 0;

		float npsSection = 0;
		float maxNPS = 0;

		int lastRate = -1;

		int lastSize = 0;
		std::vector<note> lastRow;
		float notesThisSecond = 0;

		float lastTime = 0;

		std::vector<int> npss;

		for(int is = 0; is < d.notes.size(); is += 0)
		{
			bool huh = false;

			if (is > d.notes.size())
				break;
			note n = d.notes[is];
			std::vector<note> row;

			if (n.type == Note_Tail || n.type == Note_Mine)
			{
				is += 1;
				continue;
			}

			for (int i = is; i < is + 4; i++)
			{
				if (i > d.notes.size() - 1)
					break;

				note nd = d.notes[i];

				if (nd.type == Note_Tail || nd.type == Note_Mine)
					continue;

				if (nd.beat >= n.beat - 0.01 && nd.beat <= n.beat + 0.01)
					row.push_back(nd);
			}

			is += row.size();


			if (lastBeat == -1)
			{
				lastBeat = n.beat;
				lastTime = getTimeFromBeat(lastBeat, getSegmentFromBeat(lastBeat));
			}

			int size = row.size();

			int j = 0;

			for (note nn : row)
			{
				for (note ln : lastRow)
				{
					if (nn.lane == ln.lane && (nn.beat - ln.beat <= 0.25))
					{
						j++;
					}
				}
			}

			if (lastSize == 0)
			{
				switch (size)
				{
				case 1:
					notesThisSecond++;
					singles++;
					break;
				case 2:
					notesThisSecond += 2;
					jumps++;
					break;
				case 3:
					notesThisSecond += 3;
					hands++;
					break;
				case 4:
					notesThisSecond += 4;
					quads++;
					break;
				}
				allNotes++;
			}
			else
			{
				switch (size)
				{
				case 1:

					// jacks/singles
					notesThisSecond++;

					if (j != 0)
						jacks++;
					else
						singles++;

					break;
				case 2:

					notesThisSecond+= 2;
					// gluts/jacks/chordjacks and jumps

					switch (lastRow.size())
					{
					case 1:

						if (j != 0)
							jacks++;
						else
							jumps++;
						break;
					case 2:

						switch (j)
						{
						case 0:
							jumps++;
							break;
						case 1:
							jacks++;
							break;
						case 2:
							chordJacks++;
							break;
						}
						break;
					}

					break;
				case 3:
					notesThisSecond += 3;
					// hands/jacks/chordjacks

					switch (lastRow.size())
					{
					case 1:

						if (j != 0)
							jacks++;
						else
							hands++;
						break;
					case 2:

						switch (j)
						{
						case 0:
							hands++;
							break;
						case 1:
							jacks++;
							break;
						case 2:
							chordJacks++;
							break;
						}
						break;
					case 3:
						switch (j)
						{
						case 0:
							hands++;
							break;
						case 1:
							jacks++;
							break;
						case 2:
						case 3:
							chordJacks++;
							break;
						}
						break;
					case 4:
						chordJacks++;
						break;
					}
					break;
				case 4:
					// quads/jacks/chordjacks
					notesThisSecond += 4;
					switch (lastRow.size())
					{
					case 1:
						if (j != 0)
							jacks++;
						else
							quads++;
						break;
					case 2:
						chordJacks++;
						break;
					case 3:
						chordJacks++;
						break;
					case 4:
						jacks++;
						break;
					}
					break;
				}
				allNotes++;
			}

			differentRowNotes++;

			lastSize = size;
			lastRow = row;

			float nTime = getTimeFromBeat(n.beat, getSegmentFromBeat(n.beat));

			if (nTime > lastTime + 1000)
			{
				npsSection = notesThisSecond;
				npss.push_back(npsSection);
				if (npsSection > maxNPS)
					maxNPS = npsSection;
				lastTime = getTimeFromBeat(n.beat, getSegmentFromBeat(n.beat));
				notesThisSecond = 0;
			}

			if (differentRowNotes >= 4 && n.beat < lastBeat + 4) {
				curMeasure++;
				measures++;

				lastBeat = n.beat;
				float avgSingles = singles / allNotes;
				float avgJumps = jumps / allNotes;
				float avgHands = hands / allNotes;
				float avgQuads = quads / allNotes;
				float avgChordJacks = chordJacks / allNotes;
				float avgJacks = jacks / allNotes;
				float avgGluts = gluts / allNotes;

				if (avgChordJacks < 0.1 && avgJacks < 0.4 && (avgHands > 0 || avgQuads > 0))
					handstreamMeasures++;
				else if (avgJumps > 0 && avgSingles > 0 && avgChordJacks < 0.5 && avgJacks < 0.5)
					jumpstreamMeasures++;
				else if (avgChordJacks > 0 && avgSingles < 0.9 && avgJacks < 0.2)
					chordjackMeasures++;
				else if (avgJacks > 0 && avgSingles < 0.75 && avgChordJacks < 0.5)
					jackMeasures++;
				else if (avgSingles > 0 && chordJacks == 0 && avgJacks == 0)
					streamMeasures++;
				else
					technicalMeasures++;

				npsSection = 0;
				jumps = 0;
				hands = 0;
				singles = 0;
				quads = 0;
				jacks = 0;
				gluts = 0;
				allNotes = 0;
				differentRowNotes = 0;
			}
			else if (n.beat > lastBeat + 4)
			{
				lastSize = 0;
				curMeasure++;
				measures++;

				lastBeat = n.beat;
				float avgSingles = singles / allNotes;
				float avgJumps = jumps / allNotes;
				float avgHands = hands / allNotes;
				float avgQuads = quads / allNotes;
				float avgChordJacks = chordJacks / allNotes;
				float avgJacks = jacks / allNotes;
				float avgGluts = gluts / allNotes;

				if (avgChordJacks < 0.1 && avgJacks < 0.4 && (avgHands > 0 || avgQuads > 0))
					handstreamMeasures++;
				else if (avgJacks > 0.2)
					jackMeasures++;
				else if (avgJumps > 0 && avgSingles > 0 && avgChordJacks < 0.5 && avgJacks < 0.5)
					jumpstreamMeasures++;
				else if (avgChordJacks > 0 && avgSingles < 0.2 && avgJacks < 0.8)
					chordjackMeasures++;
				else if (avgSingles > 0 && chordJacks == 0 && avgJacks == 0)
					streamMeasures++;
				else
					technicalMeasures++;

				differentRowNotes = 0;
				npsSection = 0;
				jumps = 0;
				hands = 0;
				singles = 0;
				quads = 0;
				jacks = 0;
				gluts = 0;
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
		if (technicalMeasures != 0)
			in.technical = technicalMeasures / measures;

		if (npss.size() > 0)
		{
			in.averageNPS = std::reduce(npss.begin(), npss.end()) / npss.size();
			in.maxNPS = maxNPS;
		}

		float sum = in.stream + in.jumpstream + in.chordjack + in.handstream + in.jacks + in.technical;

		if (sum < 1)
		{
			if (in.stream > (in.jumpstream + in.chordjack + in.handstream + in.jacks + in.technical))
				in.stream += (1 - sum);
			else if (in.chordjack > (in.jumpstream + in.stream + in.handstream + in.jacks + in.technical))
				in.chordjack += (1 - sum);
			else if (in.handstream > (in.jumpstream + in.stream + in.chordjack + in.jacks + in.technical))
				in.handstream += (1 - sum);
			else if (in.jacks > (in.jumpstream + in.stream + in.chordjack + in.handstream + in.technical))
				in.handstream += (1 - sum);
			else if (in.jumpstream > (in.handstream + in.stream + in.chordjack + in.jacks + in.technical))
				in.jumpstream += (1 - sum);
			else
				in.technical += (1 - sum);
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