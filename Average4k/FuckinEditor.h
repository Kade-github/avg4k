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

struct thingy {
	Text* text;
	AvgRect* background;
};

struct line {
	float beat;
	float time;
	AvgRect* rect;
	Text* text;
};

class FuckinEditor : public Menu
{
public:
	void create() override;
	Channel* song;
	bool songPlaying = false;
	AvgGroup* gameplay;
	AvgGroup* top;

	void update(Events::updateEvent event) override;
	void imguiUpdate(float elapsed) override;
	std::vector<ReceptorObject*> fuck;
	std::vector<editorWindow> windows;

	std::vector<line> beatLines;
	std::vector<thingy> sideStuff;

	void keyDown(SDL_KeyboardEvent event) override;
	void keyUp(SDL_KeyboardEvent event) override;

	void mouseWheel(float wheel) override;

	void loadNotes(difficulty diff);

	void regenBeatLines(Chart* selectedChart)
	{
		int lastBeat = -1;
		for (int i = 0; i < song->length; i++)
		{
			bpmSegment seg = selectedChart->getSegmentFromTime(i);
			float beat = selectedChart->getBeatFromTime(i, seg);
			float diff = i;

			float bps = (Game::save->GetDouble("scrollspeed") / 60);

			float noteOffset = (bps * (diff / 1000)) * (64 * Game::save->GetDouble("Note Size"));

			if ((int)beat % 4 == 0 && lastBeat != (int)beat)
			{
				lastBeat = (int)beat;
				AvgRect* rect = new AvgRect(((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) - 4, fuck[0]->y + noteOffset, 0,2);
				rect->c = { 255,255,255 };
				rect->w = (((Game::gameWidth / 2) - ((64 * Game::save->GetDouble("Note Size") + 12) * 2)) + ((64 * Game::save->GetDouble("Note Size") + 12) * 3) - rect->x) + (68 * Game::save->GetDouble("Note Size") + 12);
				line l;
				l.rect = rect;
				l.time = i;
				l.beat = beat;
				l.text = new Text(rect->x - 10, rect->y, std::to_string((int)beat), 16, "Futura Bold");
				gameplay->add(rect);
				gameplay->add(l.text);
				beatLines.push_back(l);
			}
		}
	}

	void generateNoteObject(note n, difficulty diff, Chart* selectedChart, std::vector<NoteObject*>& notes)
	{
		if (n.type == noteType::Note_Tail || n.type == noteType::Note_Mine)
			return;
		bool downscroll = false;
		NoteObject* object = new NoteObject();
		object->fboMode = false;
		object->currentChart = selectedChart;
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
		rect.w = 64 * Game::save->GetDouble("Note Size");
		rect.h = 64 * Game::save->GetDouble("Note Size");
		object->rect = rect;

		note tail;

		bpmSegment bruh = selectedChart->getSegmentFromBeat(object->beat);

		float wh = selectedChart->getTimeFromBeat(n.beat, bruh);

		float bps = (Game::save->GetDouble("scrollspeed") / 60);


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

				object->endTime = selectedChart->getTimeFromBeatOffset(nn.beat + nstopBeatOffset, noteSeg);
				tail = nn;
				break;
			}
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

				float noteOffset = (bps * (diff / 1000)) * (64 * Game::save->GetDouble("Note Size"));

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
					otherOne = yDiff <= -(64 * Game::save->GetDouble("Note Size"));
				else
					otherOne = yDiff >= 64 * Game::save->GetDouble("Note Size");

				if (otherOne || object->heldTilings.size() == 0)
				{
					object->holdHeight += 64 * Game::save->GetDouble("Note Size");
					holdTile tile;
					SDL_FRect rect;
					tile.active = true;
					tile.fucked = false;
					rect.y = y;
					rect.x = 0;
					rect.w = 64 * Game::save->GetDouble("Note Size");
					rect.h = 68 * Game::save->GetDouble("Note Size");
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