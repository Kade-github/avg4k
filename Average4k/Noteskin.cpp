#include "Noteskin.h"
#include "Game.h"
#include <windows.h>
#include <SDL_image.h>
#include "Helpers.h"

std::string Noteskin::type = "arrow";

noteskin_asset* Noteskin::asset = NULL;

Texture* getAsset(std::string path)
{
	// stbi cannot find these for some reason??
	Texture* texture = Texture::createWithImage(path);

	return texture;
}

void Noteskin::resetNoteskin(noteskin_asset* as)
{
	// TODO: FIGURE OUT HOW TO SAFLEY DELETE SKINS
	// CALLING "delete as" OR "delete as->'any tex'" DOESN'T WORK
	// IT JUST ACCESS VIOLATIONS SOMETIMES
	// IDK WHY
}


bool convertStringBool(std::string text) {
	return text == "false" ? false : true;
}

noteskin_asset* loadSkin(noteskin_asset* as, std::string typ) {
	as = new noteskin_asset();
	Noteskin::type = typ;
	as->fourth = getAsset("assets/noteskin/" + Noteskin::type + "/4th.png");
	as->eighth = getAsset("assets/noteskin/" + Noteskin::type + "/8th.png");
	as->twelfth = getAsset("assets/noteskin/" + Noteskin::type + "/12th.png");
	as->sixteenth = getAsset("assets/noteskin/" + Noteskin::type + "/16th.png");
	as->thirty2nd = getAsset("assets/noteskin/" + Noteskin::type + "/32nd.png");
	as->none = getAsset("assets/noteskin/" + Noteskin::type + "/idfk.png");
	as->receptor = getAsset("assets/noteskin/" + Noteskin::type + "/Receptor.png");
	as->hold = getAsset("assets/noteskin/" + Noteskin::type + "/hold.png");
	as->holdend = getAsset("assets/noteskin/" + Noteskin::type + "/holdend.png");
	as->light = getAsset("assets/noteskin/" + Noteskin::type + "/lit.png");

	std::ifstream config("assets/noteskin/" + Noteskin::type + "/config.skin");
	std::string line;
	if (config.is_open())
	{
		while (getline(config, line))
		{
			if (line.find("#") != std::string::npos)
				continue;
			std::vector<std::string> split = Chart::split(line, ' ');
			std::string first = split[0].substr(0,split[0].size() - 1);
			std::string second = split[1];
			std::transform(first.begin(), first.end(), first.begin(), Helpers::asciitolower);
			std::transform(second.begin(), second.end(), second.begin(), Helpers::asciitolower);
			if (first == "rotate")
				as->rotate = convertStringBool(second);
			if (first == "bounce")
				as->bounce = convertStringBool(second);
			if (first == "shrink")
				as->shrink = convertStringBool(second);
			if (first == "path")
				as->skinpath = second;
		}
		config.close();
	}
	as->name = Noteskin::type;
	return as;
}

noteskin_asset* Noteskin::getNoteskin()
{
	if (type != Game::save->GetString("Noteskin"))
	{
		type = Game::save->GetString("Noteskin");
		if (!std::filesystem::exists("assets/noteskin/" + type))
		{
			Game::save->SetString("Noteskin", "arrow");
			type = "arrow";
			Game::showErrorWindow("Noteskin error!", "Falling back to 'arrow'", false);
		}

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
