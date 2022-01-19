#include "Noteskin.h"
#include "Game.h"
#include <windows.h>
#include <SDL_image.h>

std::string Noteskin::type = "arrow";

noteskin_asset* Noteskin::asset = NULL;

Texture* getAsset(std::string path)
{
	// stbi cannot find these for some reason??
	Texture* texture = Texture::createFromSurface(IMG_Load(path.c_str()), true);

	return texture;
}

void Noteskin::resetNoteskin(noteskin_asset* as)
{
	if (as)
	{
		delete as->fourth;
		delete as->eighth;
		delete as->hold;
		delete as->sixteenth;
		delete as->twelfth;
		delete as->holdend;
		delete as->receptor;
		delete as->none;
		delete as->thirty2nd;
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
	as->fourth = getAsset("assets/noteskin/" + type + "/4th.png");
	as->eighth = getAsset("assets/noteskin/" + type + "/8th.png");
	as->twelfth = getAsset("assets/noteskin/" + type + "/12th.png");
	as->sixteenth = getAsset("assets/noteskin/" + type + "/16th.png");
	as->thirty2nd = getAsset("assets/noteskin/" + type + "/32nd.png");
	as->hold = getAsset("assets/noteskin/" + type + "/hold.png");
	as->none = getAsset("assets/noteskin/" + type + "/idfk.png");
	as->receptor = getAsset("assets/noteskin/" + type + "/Receptor.png");
	as->holdend = getAsset("assets/noteskin/" + type + "/holdend.png");
	as->light = getAsset("assets/noteskin/" + type + "/lit.png");

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
