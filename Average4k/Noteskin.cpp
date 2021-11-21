#include "Noteskin.h"
#include "Game.h"
#include <windows.h>
#include <SDL_image.h>

std::string Noteskin::type = "arrow";

SDL_Texture* getAsset(std::string path, SDL_Renderer* renderer)
{
	SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());

	return texture;
}

noteskin_asset* Noteskin::getNoteskin()
{
	static noteskin_asset* asset;
	if (!asset)
	{
		asset = new noteskin_asset();
		asset->fourth = getAsset("assets/noteskin/" + type + "/4th.png", Game::renderer);
		asset->eighth = getAsset("assets/noteskin/" + type + "/8th.png", Game::renderer);
		asset->twelfth = getAsset("assets/noteskin/" + type + "/12th.png", Game::renderer);
		asset->sixteenth = getAsset("assets/noteskin/" + type + "/16th.png", Game::renderer);
		asset->thirty2nd = getAsset("assets/noteskin/" + type + "/32nd.png", Game::renderer);
		asset->hold = getAsset("assets/noteskin/" + type + "/hold.png", Game::renderer);
		asset->none = getAsset("assets/noteskin/" + type + "/idfk.png", Game::renderer);
		asset->receptor = getAsset("assets/noteskin/" + type + "/Receptor.png", Game::renderer);
	}
	return asset;
}
