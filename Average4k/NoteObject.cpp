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


		SDL_Rect l;
		l.x = 0;
		l.y = (receptor.y + 32);
		l.w = Game::gameWidth;
		l.h = Game::gameHeight - (receptor.y + 32);

		int heightForBruh = 0;

		int activeH = 0;

		for (int i = 0; i < heldTilings.size(); i++)
		{
			bool condition = (
				downscroll ?
				heldTilings[i].rect.y > (receptor.y - 32)
				: heldTilings[i].rect.y < (receptor.y + 32));

			if (heldTilings[i].active || condition)
				activeH++;
		}
		if (activeH != holdsActive)
			holdsActive = activeH;


		SDL_Rect clipThingy;

		SDL_Texture* holdTexture = SDL_CreateTexture(Game::renderer,NULL,SDL_TEXTUREACCESS_TARGET,64, holdHeight);

		if (type == Note_Head)
		{
			int size = heldTilings.size();
			SDL_SetRenderTarget(Game::renderer, holdTexture);
			SDL_SetTextureBlendMode(holdTexture, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 0);
			SDL_RenderFillRect(Game::renderer, NULL);
			clipThingy.x = 0;
			clipThingy.y = 32;
			clipThingy.w = 64;
			clipThingy.h = holdHeight;
			if (downscroll)
			{
				clipThingy.y -= 32;
				clipThingy.h += 32;
			}
			for (int i = 0; i < size; i++)
			{
				holdTile& tile = heldTilings[i];
				auto whHold = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat));
			
				float diff2 = ((whHold)) - position;
				tile.rect.x = 0;

				noteOffset = (bps * (diff2 / 1000)) * 64;
				
				tile.rect.y = noteOffset;

				/*if (downscroll)
				{
					if (i != 0)
						tile.rect.y = heldTilings[i - 1].rect.y - 45;
					else
						tile.rect.y = (rect.y - noteOffset) - 45;
					if (i == heldTilings.size() - 1 && i != 0)
						tile.rect.y = tile.rect.y - 25;
				}
				else
				{
					if (i != 0)
						tile.rect.y = heldTilings[i - 1].rect.y + 45;
					else
						tile.rect.y = (rect.y + noteOffset) + 45;

					if (i == heldTilings.size() - 1 && i != 0)
						tile.rect.y = tile.rect.y + 25;
					
				}*/

				// pov you're clipping

				if (downscroll)
				{
					l.y = 0;
					l.h = receptor.y + 32;
				}
			}
			SDL_RenderSetClipRect(Game::renderer, &clipThingy);
			if (size != 0)
			{
				int nomr = heldTilings[0].rect.y;
				for (int i = 0; i < size; i++)
				{
					holdTile& tile = heldTilings[i];
					tile.rect.y = tile.rect.y - nomr;
				}
			}

			for (int i = 0; i < size; i++)
			{
				holdTile& tile = heldTilings[i];

				if (tile.active)
				{
					if (i != size - 1)
					{
						if (!downscroll)
							SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->hold, NULL, &tile.rect, 0, NULL, SDL_FLIP_NONE);
						else
							SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->hold, NULL, &tile.rect, 0, NULL, SDL_FLIP_VERTICAL);
					}
				}
			}

			if (size > 1)
			{
				holdTile tile = heldTilings.back();

				
				SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->holdend, NULL, &tile.rect, 0, NULL, SDL_FLIP_VERTICAL);
				
			}

			SDL_RenderSetClipRect(Game::renderer, NULL);
			SDL_SetRenderTarget(Game::renderer, Game::mainCamera->cameraTexture);
		}

		if (heldTilings.size() != 0)
		{
			if (clipHold)
				SDL_RenderSetClipRect(Game::renderer, &l);

			SDL_Rect copy = clipThingy;
			copy.x = rect.x;
			copy.y = rect.y;
			if (downscroll)
				copy.y -= (holdHeight - 64);

			SDL_SetRenderDrawBlendMode(Game::renderer, SDL_BLENDMODE_BLEND);
			SDL_RenderCopyEx(Game::renderer, holdTexture, NULL, &copy,0,NULL,downscroll ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);

			if (clipHold)
				SDL_RenderSetClipRect(Game::renderer, NULL);
		}


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

			SDL_RenderDrawRect(Game::renderer, &l);
			if (holdTexture && heldTilings.size() != 0)
			{
				SDL_Rect copy = clipThingy;
				copy.x = rect.x;
				copy.y = rect.y + copy.y;
				if (downscroll)
					copy.y -= (holdHeight - 64);
				SDL_RenderDrawRect(Game::renderer, &copy);
			}
		}
		if (holdTexture)
			SDL_DestroyTexture(holdTexture);

}
