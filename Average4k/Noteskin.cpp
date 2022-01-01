#include "Noteskin.h"
#include "Game.h"
#include <windows.h>
#include <SDL_image.h>

std::string Noteskin::type = "arrow";

noteskin_asset* Noteskin::asset = NULL;

SDL_Texture* getAsset(std::string path, SDL_Renderer* renderer)
{
	SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());

	return texture;
}

void Noteskin::resetNoteskin(noteskin_asset* as)
{
	if (as)
	{
		SDL_DestroyTexture(as->fourth);
		SDL_DestroyTexture(as->eighth);
		SDL_DestroyTexture(as->hold);
		SDL_DestroyTexture(as->sixteenth);
		SDL_DestroyTexture(as->twelfth);
		SDL_DestroyTexture(as->holdend);
		SDL_DestroyTexture(as->receptor);
		SDL_DestroyTexture(as->none);
		SDL_DestroyTexture(as->thirty2nd);
		free(as);
		as = NULL;
	}
}

char asciitolower(char in) {
	if (in <= 'Z' && in >= 'A')
		return in - ('Z' - 'z');
	return in;
}

noteskin_asset* loadSkin(noteskin_asset* as, std::string type) {
	as = new noteskin_asset();
	as->fourth = getAsset("assets/noteskin/" + type + "/4th.png", Game::renderer);
	as->eighth = getAsset("assets/noteskin/" + type + "/8th.png", Game::renderer);
	as->twelfth = getAsset("assets/noteskin/" + type + "/12th.png", Game::renderer);
	as->sixteenth = getAsset("assets/noteskin/" + type + "/16th.png", Game::renderer);
	as->thirty2nd = getAsset("assets/noteskin/" + type + "/32nd.png", Game::renderer);
	as->hold = getAsset("assets/noteskin/" + type + "/hold.png", Game::renderer);
	as->none = getAsset("assets/noteskin/" + type + "/idfk.png", Game::renderer);
	as->receptor = getAsset("assets/noteskin/" + type + "/Receptor.png", Game::renderer);
	as->holdend = getAsset("assets/noteskin/" + type + "/holdend.png", Game::renderer);
	as->light = getAsset("assets/noteskin/" + type + "/lit.png", Game::renderer);

	std::ifstream config("assets/noteskin/" + type + "/config.skin");
	std::string line;
	if (config.is_open())
	{
		while (getline(config, line))
		{
			std::vector<std::string> split = Chart::split(line, ' ');
			std::string first = split[0].substr(0,split[0].size() - 1);
			std::string second = split[1];
			std::transform(first.begin(), first.end(), first.begin(), asciitolower);
			std::transform(second.begin(), second.end(), second.begin(), asciitolower);
			if (first == "rotate")
				as->rotate = (second == "false" ? false : true);
		}
		config.close();
	}
	return as;
}

noteskin_asset* Noteskin::getNoteskin()
{
	if (type != Game::save->GetString("Noteskin"))
	{
		type = Game::save->GetString("Noteskin");
		std::cout << "creating noteskin " << "assets/noteskin/" + type << std::endl;
		asset = loadSkin(asset, type);
		std::cout << "done noteskin" << std::endl;
	}
	else if (!asset)
	{
		std::cout << "creating noteskin " << "assets/noteskin/" + type << std::endl;
		asset = loadSkin(asset, type);
		std::cout << "done noteskin" << std::endl;
	}
	return asset;
}
