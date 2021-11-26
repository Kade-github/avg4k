#include "NoteObject.h"
#include "Gameplay.h"
#include <mutex>
#include "Judge.h"

NoteObject::NoteObject(){}

void NoteObject::destroy()
{
	destroyed = true;
	delete this;
}

void NoteObject::draw(float position, double b, SDL_FRect receptor)
{
		bpmSegment bruh = MainMenu::currentChart->getSegmentFromBeat(beat);

		float wh = MainMenu::currentChart->getTimeFromBeat(beat, bruh);

		float diff = wh - ((position + (Game::save->GetDouble("offset") / 1000)) / 1000);

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

		if (MainMenu::currentChart->meta.chartType == 1) // osu/quaver
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
				auto whHold = MainMenu::currentChart->getTimeFromBeat(tile.beat, MainMenu::currentChart->getSegmentFromBeat(tile.beat));
			

				float diff2 = (whHold + (Game::save->GetDouble("offset") / 1000)) - wh;
				tile.rect.x = rect.x;

				noteOffset = (bps * diff2) * 64;

				if (downscroll)
					tile.rect.y = (rect.y - noteOffset) - 96;
				else
					tile.rect.y = (rect.y + noteOffset) + 96;

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
					SDL_RenderCopyF(Game::renderer, Gameplay::noteskin->hold, NULL, &tile.rect);
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
