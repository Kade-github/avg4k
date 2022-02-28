#pragma once
#include "Menu.h"
#include "includes.h"
#include "AvgSprite.h";
#include "Chart.h"
#include "imgui.h"
#include "ReceptorObject.h"
#include "NoteObject.h"
#include "Text.h"

typedef void(__cdecl* drawCall)();
struct editorWindow {
	std::string title;
	ImVec2 size;
	drawCall draw;
	bool shouldDraw;
	float xOff = 0;
};

struct waveformSeg {
	AvgSprite* sprite;
	float time;
	float length;
	int offset;
};

struct thingy {
	Text* text;
	AvgRect* background;
	float beat;
	float time;
};

struct line {
	float beat;
	float time;
	AvgRect* rect;
	Text* text;
	float freqR;
};

class FuckinEditor : public Menu
{
public:
	float noteZoom = 1;
	AvgSprite* lunder;
	AvgSprite* lunderBorder;
	void create() override;
	Channel* song;
	std::vector<Channel*> clapChannels;
	std::vector<float> beatsClapped;
	bool findClapped(float beat)
	{
		for (float f : beatsClapped)
		{
			if (f == beat)
				return true;
		}
		return false;
	}
	bool songPlaying = false;
	AvgGroup* gameplay;
	AvgGroup* lines;
	AvgGroup* wave;
	AvgGroup* top;

	std::vector<waveformSeg> waveform;

	void update(Events::updateEvent event) override;
	void imguiUpdate(float elapsed) override;
	std::vector<ReceptorObject*> fuck;
	std::vector<editorWindow> windows;

	bool focused = false;

	std::vector<line> beatLines;
	std::vector<line> snapBeat;
	std::vector<thingy> sideStuff;

	std::vector<line> waveFormStuff;

	void keyDown(SDL_KeyboardEvent event) override;
	void keyUp(SDL_KeyboardEvent event) override;

	void mouseWheel(float wheel) override;

	void loadNotes(difficulty diff);

	void generateWaveForm();

	void generateSnapLines(Chart* selectedChart, float snapDiv)
	{
		for (line thing : snapBeat)
		{
			lines->removeObj(thing.rect);
			//gameplay->removeObj(thing.text);
		}
		snapBeat.clear();
		int lastBeat = -1;
		for (int i = 0; i < song->length; i++)
		{
			bpmSegment seg = selectedChart->getSegmentFromTime(i);
			float beat = selectedChart->getBeatFromTime(i, seg);
			if (beat < 0)
				continue;

			float diff = i;

			float bps = (Game::save->GetDouble("scrollspeed") / 60);

			float noteOffset = (bps * (diff / 1000)) * (64 * noteZoom);

			if (lastBeat != (int)beat)
			{
				lastBeat = (int)beat;
				AvgRect* rect = new AvgRect(((Game::gameWidth / 2) - ((64 * noteZoom + 12) * 2)) - 4, fuck[0]->y + noteOffset + (32 * noteZoom), 0, 2);
				rect->c = { 128,128,128 };
				rect->w = (((Game::gameWidth / 2) - ((64 * noteZoom + 12) * 2)) + ((64 * noteZoom + 12) * 3) - rect->x) + (68 * noteZoom + 12);
				line l;
				l.rect = rect;
				l.time = i;
				l.beat = beat;
				//l.text = new Text(rect->x - 10, rect->y, std::to_string((int)snappedBeat), 16, "Futura Bold");
				lines->add(rect);
				//gameplay->add(l.text);
				snapBeat.push_back(l);
			}
		}
	}

	float getWaveLength(float second, Chart* selectedChart)
	{
		float time = second * 1000;

		float bps = (Game::save->GetDouble("scrollspeed") / 60);

		return (bps * (time / 1000)) * (64 * noteZoom);
	}

	void regenBeatLines(Chart* selectedChart)
	{
		for (line thing : beatLines)
		{
			lines->removeObj(thing.rect);
			lines->removeObj(thing.text);
		}

		beatLines.clear();

		int lastBeat = -1;
		for (int i = 0; i < song->length; i++)
		{
			bpmSegment seg = selectedChart->getSegmentFromTime(i);
			float beat = selectedChart->getBeatFromTime(i, seg);
			if (beat < 0)
				continue;
			float diff = i;

			float bps = (Game::save->GetDouble("scrollspeed") / 60);

			float noteOffset = (bps * (diff / 1000)) * (64 * noteZoom);

			if (((int)beat % 4 == 0 || lastBeat == -1) && lastBeat != (int)beat)
			{
				lastBeat = (int)beat;
				AvgRect* rect = new AvgRect(((Game::gameWidth / 2) - ((64 * noteZoom + 12) * 2)) - 4, fuck[0]->y + noteOffset + (32 * noteZoom), 0,2);
				rect->c = { 255,255,255 };
				rect->w = (((Game::gameWidth / 2) - ((64 * noteZoom + 12) * 2)) + ((64 * noteZoom + 12) * 3) - rect->x) + (68 * noteZoom + 12);
				line l;
				l.rect = rect;
				l.time = i;
				l.beat = beat;
				l.text = new Text(rect->x - 10, rect->y, std::to_string((int)beat), 16, "Futura Bold");
				lines->add(rect);
				lines->add(l.text);
				beatLines.push_back(l);
			}
		}
	}

	void regenThings(Chart* selectedChart)
	{
		for (thingy& thing : sideStuff)
		{
			gameplay->removeObj(thing.background);
			gameplay->removeObj(thing.text);
		}

		sideStuff.clear();

		for (bpmSegment seg : selectedChart->meta.bpms)
		{
			float beat = seg.startBeat;
			float diff = selectedChart->getTimeFromBeat(beat, seg);

			float bps = (Game::save->GetDouble("scrollspeed") / 60);

			float noteOffset = (bps * (diff / 1000)) * (64 * noteZoom);

			AvgRect* rect = new AvgRect(((Game::gameWidth / 2) - ((64 * noteZoom + 12) * 2)) - 4, fuck[0]->y + noteOffset, 0, 25);
			rect->c = { 50, 165, 50 };
			rect->w = (((Game::gameWidth / 2) - ((64 * noteZoom + 12) * 2)) + ((64 * noteZoom + 12) * 3) - rect->x) + (68 * noteZoom + 12);
			rect->x += rect->w + 25;
			rect->w = 95;

			thingy t;
			t.beat = beat;
			t.time = diff;
			t.background = rect;

			char buffer[12];

			sprintf_s(buffer, "%.2f", seg.bpm);

			t.text = new Text(rect->x + 4, rect->y + 2, "BPM " + std::string(buffer), 16, "Futura Bold");
			t.background->w = t.text->surfW + 12;
			t.background->h = t.text->surfH + 4;
			gameplay->add(rect);
			gameplay->add(t.text);
			sideStuff.push_back(t);
		}

		for (stopSegment seg : selectedChart->meta.stops)
		{
			float beat = seg.beat;
			float diff = selectedChart->getTimeFromBeat(beat, selectedChart->getSegmentFromBeat(beat));

			float bps = (Game::save->GetDouble("scrollspeed") / 60);

			float noteOffset = (bps * (diff / 1000)) * (64 * noteZoom);

			AvgRect* rect = new AvgRect(((Game::gameWidth / 2) - ((64 * noteZoom + 12) * 2)) - 4, fuck[0]->y + noteOffset, 0, 25);
			rect->c = { 138,43,226 };
			rect->w = (((Game::gameWidth / 2) - ((64 * noteZoom + 12) * 2)) + ((64 * noteZoom + 12) * 3) - rect->x) + (68 * noteZoom + 12);
			rect->x += rect->w + 25;
			rect->w = 95;

			thingy t;
			t.beat = beat;
			t.time = diff;
			t.background = rect;

			char buffer[12];

			sprintf_s(buffer, "%.2f", seg.length / 1000);

			t.text = new Text(rect->x + 4, rect->y + 2, "Stop " + std::string(buffer) + "s", 16, "Futura Bold");
			t.background->w = t.text->surfW + 12;
			t.background->h = t.text->surfH + 4;
			gameplay->add(rect);
			gameplay->add(t.text);
			sideStuff.push_back(t);
		}
	}

	void generateNoteObject(note n, difficulty diff, Chart* selectedChart, std::vector<NoteObject*>& notes, bool findTail = true)
	{
		if (n.type == noteType::Note_Tail || n.type == noteType::Note_Mine)
			return;
		bool downscroll = false;
		NoteObject* object = new NoteObject();
		object->fboMode = false;
		object->size = noteZoom;
		object->fboX = fuck[n.lane]->x;
		object->currentChart = selectedChart;
		object->connected = &n;
		SDL_FRect rect;
		object->wasHit = false;
		object->clapped = false;
		object->active = true;

		bpmSegment preStopSeg = selectedChart->getSegmentFromBeat(n.beat);

		float stopOffset = selectedChart->getStopOffsetFromBeat(n.beat);

		double stopBeatOffset = (stopOffset / 1000) * (preStopSeg.bpm / 60);

		object->stopOffset = stopBeatOffset;

		object->beat = (double)n.beat + stopBeatOffset;
		object->lane = n.lane;
		object->connectedReceptor = fuck[n.lane];
		object->type = n.type;
		object->endTime = -1;
		object->endBeat = -1;

		bpmSegment noteSeg = selectedChart->getSegmentFromBeat(object->beat);

		object->time = selectedChart->getTimeFromBeatOffset(object->beat, noteSeg);
		rect.y = Game::gameHeight + 400;
		rect.x = 0;
		rect.w = 64 * noteZoom;
		rect.h = 64 * noteZoom;
		object->rect = rect;

		note tail;

		bpmSegment bruh = selectedChart->getSegmentFromBeat(object->beat);

		float wh = selectedChart->getTimeFromBeat(n.beat, bruh);

		float bps = (Game::save->GetDouble("scrollspeed") / 60);

		if (findTail)
		{
			if (object->type == Note_Head)
			{
				for (int i = 0; i < diff.notes.size(); i++)
				{
					note& nn = diff.notes[i];
					if (nn.type != Note_Tail)
						continue;
					if (nn.lane != object->lane || nn.connectedBeat != n.beat)
						continue;

					bpmSegment npreStopSeg = selectedChart->getSegmentFromBeat(nn.beat);

					float nstopOffset = selectedChart->getStopOffsetFromBeat(nn.beat);

					double nstopBeatOffset = (nstopOffset / 1000) * (npreStopSeg.bpm / 60);

					object->endBeat = nn.beat + nstopBeatOffset;
					object->tailBeat = nn.beat;

					object->endTime = selectedChart->getTimeFromBeatOffset(nn.beat + nstopBeatOffset, noteSeg);
					tail = nn;
					break;
				}
			}
		}
		else
		{
			note nn;
			nn.beat = n.connectedBeat;
			nn.connectedBeat = -1;
			nn.type = noteType::Note_Tail;
			nn.lane = n.lane;
			object->tailBeat = nn.beat;
			tail = nn;
			object->endTime = selectedChart->getTimeFromBeatOffset(n.connectedBeat, noteSeg);
		}

		float time = SDL_GetTicks();

		if (object->type == Note_Head)
		{

			for (int i = std::floorf(object->time); i < std::floorf(object->endTime); i++)
			{
				bpmSegment holdSeg = selectedChart->getSegmentFromTime(i);

				double beat = selectedChart->getBeatFromTimeOffset(i, holdSeg);

				float whHold = selectedChart->getTimeFromBeatOffset(beat, holdSeg);

				float diff = whHold - (object->time);

				float noteOffset = (bps * (diff / 1000)) * (64 * noteZoom);

				float y = 0;
				float yDiff = 0;
				if (object->heldTilings.size() != 0)
				{
					if (downscroll)
						y = object->rect.y - noteOffset;
					else
						y = object->rect.y + noteOffset;
					yDiff = y - object->heldTilings.back().rect.y;
				}
				else
				{
					if (downscroll)
						y = object->rect.y - noteOffset;
					else
						y = object->rect.y + noteOffset;
					yDiff = y - object->rect.y;
				}

				bool otherOne = false;

				if (downscroll)
					otherOne = yDiff <= -(64 * noteZoom);
				else
					otherOne = yDiff >= 64 * noteZoom;

				if (otherOne || object->heldTilings.size() == 0)
				{
					object->holdHeight += 64 * noteZoom;
					holdTile tile;
					SDL_FRect rect;
					tile.active = true;
					tile.fucked = false;
					rect.y = y;
					rect.x = 0;
					rect.w = 64 * noteZoom;
					rect.h = 68 * noteZoom;
					tile.rect = rect;
					tile.beat = beat;
					tile.time = i;
					object->heldTilings.push_back(tile);
				}
			}
		}
		std::sort(object->heldTilings.begin(), object->heldTilings.end());
		notes.push_back(object);
		object->create();
		gameplay->add(object);
	}

	void createWindow(std::string title, ImVec2 size, drawCall draw, bool shouldDraw)
	{
		editorWindow wind;
		wind.title = title;
		wind.shouldDraw = shouldDraw;
		wind.size = size;
		wind.draw = draw;
		windows.push_back(wind);
	}

	editorWindow& findWindow(std::string title)
	{
		editorWindow found;
		for (editorWindow& wind : windows)
			if (wind.title == title)
				return wind;
		return found;
	}
};