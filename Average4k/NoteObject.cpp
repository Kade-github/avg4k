#include "NoteObject.h"
#include "Gameplay.h"
#include <mutex>
#include "Judge.h"
#include "SongSelect.h"

NoteObject::NoteObject(){}

void NoteObject::destroy()
{
	destroyed = true;
	delete this;
}

void NoteObject::draw(float position, double b, SDL_FRect receptor)
{
		bpmSegment bruh = SongSelect::currentChart->getSegmentFromBeat(beat);

		float wh = SongSelect::currentChart->getTimeFromBeat(beat, bruh);

		float diff = (wh + (Game::save->GetDouble("offset")) - (position / 1000));

		float bps = (Game::save->GetDouble("scrollspeed") / 60) / Gameplay::rate;

		float noteOffset = (bps * diff) * 64;

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

		if (type == Note_Head)
		{

			for (int i = 0; i < heldTilings.size(); i++)
			{
				holdTile& tile = heldTilings[i];
				auto whHold = SongSelect::currentChart->getTimeFromBeat(tile.beat, SongSelect::currentChart->getSegmentFromBeat(tile.beat));
			
				float diff2 = ((whHold + (Game::save->GetDouble("offset")) / 1000)) - wh;
				tile.rect.x = rect.x;

				noteOffset = (bps * diff2) * 64;
				
				if (downscroll)
					tile.rect.y = (rect.y - noteOffset);
				else
					tile.rect.y = (rect.y + noteOffset);


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

				if (tile.fucked)
					SDL_SetTextureAlphaMod(Gameplay::noteskin->hold, 60);
				else
					SDL_SetTextureAlphaMod(Gameplay::noteskin->hold, 255);

				bool condition = false;

				if (downscroll)
					condition = tile.rect.y < receptor.y;
				else
					condition = tile.rect.y > receptor.y;

				if (tile.active || tile.fucked || condition)
				{
					if (i == heldTilings.size() - 1)
					{

						if (!downscroll)
						{
							SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->holdend, NULL, &tile.rect, 0, NULL, SDL_FLIP_VERTICAL);
						}
						else
						{
							SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->holdend, NULL, &tile.rect, 0, NULL, SDL_FLIP_NONE);
						}
					}
					else
					{
						SDL_RenderCopyF(Game::renderer, Gameplay::noteskin->hold, NULL, &tile.rect);
					}
				}
			}
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

}
