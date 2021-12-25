#include "NoteObject.h"
#include "Gameplay.h"
#include <mutex>
#include <boost/algorithm/clamp.hpp>
#include "Judge.h"
#include "SongSelect.h"

NoteObject::NoteObject(){}

void NoteObject::destroy()
{
	destroyed = true;
	delete this;
}

void NoteObject::draw(float position, double b, SDL_FRect receptor, bool clipHold)
{
		bpmSegment bruh = SongSelect::currentChart->getSegmentFromBeat(beat);

		float wh = SongSelect::currentChart->getTimeFromBeat(beat, bruh);

		float diff = (wh) - (position);

		float bps = (Game::save->GetDouble("scrollspeed") / 60) / Gameplay::rate;

		float noteOffset = (bps * (diff / 1000)) * 64;

		bool downscroll = Game::save->GetBool("downscroll");

		if (downscroll)
			rect.y = (receptor.y - noteOffset);
		else
			rect.y = (receptor.y + noteOffset);

		SDL_Texture* texture;

		// get quant

		float beatRow = beat * 48;

		if (SongSelect::currentChart->meta.chartType == 1) // osu/quaver
		{
			float pos = wh - bruh.startTime;
			float bps = 60 / bruh.bpm;

			beatRow = std::roundf(48 * (pos / bps));
		}
		if (fmod(beatRow, (192 / 4)) == 0)
			texture = Gameplay::noteskin->fourth;
		else if (fmod(beatRow, (192 / 8)) == 0)
			texture = Gameplay::noteskin->eighth;
		else if (fmod(beatRow, (192 / 12)) == 0)
			texture = Gameplay::noteskin->twelfth;
		else if (fmod(beatRow, (192 / 16)) == 0)
			texture = Gameplay::noteskin->sixteenth;
		else if (fmod(beatRow, (192 / 32)) == 0)
			texture = Gameplay::noteskin->thirty2nd;
		else
			texture = Gameplay::noteskin->none;

		SDL_Rect clipThingy;

		clipThingy.x = 0;
		clipThingy.y = rect.y + 32;
		clipThingy.w = 64;
		clipThingy.h = holdHeight;
		if (downscroll)
		{
			clipThingy.y -= holdHeight;
		}

		SDL_RenderSetClipRect(Game::renderer, &clipThingy);

		for (int i = 0; i < heldTilings.size(); i++)
		{
			holdTile& tile = heldTilings[i];
			float time = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat));

			float diff2 = time - position;

			float offsetFromY = (bps * (diff2 / 1000)) * 64;
			tile.rect.y = receptor.y + offsetFromY;
			if (downscroll)
				tile.rect.y = receptor.y - offsetFromY;

			if (i != heldTilings.size() - 1)
			{
				if (!downscroll)
					SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->hold, NULL, &tile.rect, 0, NULL, SDL_FLIP_NONE);
				else
					SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->hold, NULL, &tile.rect, 0, NULL, SDL_FLIP_VERTICAL);
			}
			else
			{
				if (downscroll)
					SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->holdend, NULL, &tile.rect, 0, NULL, SDL_FLIP_NONE);
				else
					SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->holdend, NULL, &tile.rect, 0, NULL, SDL_FLIP_VERTICAL);
			}
			
		}


		SDL_RenderSetClipRect(Game::renderer, NULL);



		int activeH = 0;


		for (int i = 0; i < heldTilings.size(); i++)
		{
			holdTile& tile = heldTilings[i];
			float time = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat));

			float diff2 = time - position;
			bool condition = diff2 > -Judge::hitWindows[4];

			if (heldTilings[i].active || condition)
				activeH++;
		}
		if (activeH != holdsActive)
			holdsActive = activeH;
		

		if (active)
			switch (lane)
			{
			case 0:
				SDL_RenderCopyExF(Game::renderer, texture, NULL, &rect, 90, NULL, SDL_FLIP_NONE);
				break;
			case 1:
				SDL_RenderCopyExF(Game::renderer, texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
				break;
			case 2:
				SDL_RenderCopyExF(Game::renderer, texture, NULL, &rect, 180, NULL, SDL_FLIP_NONE);
				break;
			case 3:
				SDL_RenderCopyExF(Game::renderer, texture, NULL, &rect, -90, NULL, SDL_FLIP_NONE);
				break;
			}

		if (debug)
		{
			SDL_SetRenderDrawColor(Game::renderer, 255, 255, 255, 255);
			SDL_RenderDrawRect(Game::renderer, &clipThingy);
		}
}
