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
		float noteOffset = (0.45 * Game::save->GetDouble("scrollspeed")) + MainMenu::offset;

		float wh = MainMenu::currentChart->getTimeFromBeat(beat, MainMenu::currentChart->getSegmentFromBeat(beat)) * 1000;

		bool downscroll = Game::save->GetBool("downscroll");

		if (downscroll)
			rect.y = (receptor.y - (wh - position) * noteOffset);
		else
			rect.y = (receptor.y + (wh - position) * noteOffset);

		SDL_Texture* texture;

		// get quant

		float beatRow = beat * 48;
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
				auto whHold = MainMenu::currentChart->getTimeFromBeat(tile.beat, MainMenu::currentChart->getSegmentFromBeat(tile.beat)) * 1000;
				float diff = whHold - wh;
				tile.rect.x = rect.x;

				if (downscroll)
					tile.rect.y = (rect.y - (diff * noteOffset)) - 64;
				else
					tile.rect.y = (rect.y + (diff * noteOffset)) + 64;

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
