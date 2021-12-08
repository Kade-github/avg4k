#include "ReceptorObject.h"
#include "Gameplay.h"

void ReceptorObject::draw() {


	SDL_FRect rect;

	rect.w = 64;
	rect.h = 64;
	rect.x = x;
	rect.y = y;


	switch (type)
	{
	case 0:
		SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->receptor, NULL, &rect, 90, NULL, SDL_FLIP_NONE);
		if (lightUpTimer > 0)
		{
			SDL_SetTextureAlphaMod(Gameplay::noteskin->receptor, std::lerp(255, 0, lightUpTimer / 1000));
			SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->light, NULL, &rect, 90, NULL, SDL_FLIP_NONE);
		}

		break;
	case 1:
		SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->receptor, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
		if (lightUpTimer > 0)
		{
			SDL_SetTextureAlphaMod(Gameplay::noteskin->receptor, std::lerp(255, 0, lightUpTimer / 1000));
			SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->light, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
		}
		break;
	case 2:
		SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->receptor, NULL, &rect, 180, NULL, SDL_FLIP_NONE);
		if (lightUpTimer > 0)
		{
			SDL_SetTextureAlphaMod(Gameplay::noteskin->receptor, std::lerp(255, 0, lightUpTimer / 1000));
			SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->light, NULL, &rect, 180, NULL, SDL_FLIP_NONE);
		}
		break;
	case 3:
		SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->receptor, NULL, &rect, -90, NULL, SDL_FLIP_NONE);
		if (lightUpTimer > 0)
		{
			SDL_SetTextureAlphaMod(Gameplay::noteskin->receptor, std::lerp(255, 0, lightUpTimer / 1000));
			SDL_RenderCopyExF(Game::renderer, Gameplay::noteskin->light, NULL, &rect, -90, NULL, SDL_FLIP_NONE);
		}
		break;
	}

	if (lightUpTimer > 0)
	{
		lightUpTimer = lightUpTimer - Game::deltaTime;
	}

	SDL_SetTextureAlphaMod(Gameplay::noteskin->receptor, 255);
}