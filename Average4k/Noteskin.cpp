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
	std::transform(text.begin(), text.end(), text.begin(), Helpers::asciitolower);
	return text == "false" ? false : true;
}

noteskin_asset* loadSkin(noteskin_asset* as, std::string typ) {
	VM_START
	as = new noteskin_asset();
	Noteskin::type = typ;

	std::string left;
	std::string right;
	std::string up;
	std::string down;


	std::string sparrow;

	bool useSparrow = false;
	std::ifstream config("assets/noteskin/" + Noteskin::type + "/config.skin");
	std::string line;
	if (config.is_open())
	{
		while (getline(config, line))
		{
			if (line.find("#") != std::string::npos || line.size() == 0)
				continue;
			std::vector<std::string> split = Chart::split(line, ' ');
			std::string first = split[0].substr(0,split[0].size() - 1);
			std::string second = split[1];

			std::string secondSpaces = line.substr(first.size() + 2);

			std::transform(first.begin(), first.end(), first.begin(), Helpers::asciitolower);
			if (first == "rotate")
				as->rotate = convertStringBool(second);
			if (first == "bounce")
				as->bounce = convertStringBool(second);
			if (first == "shrink")
				as->shrink = convertStringBool(second);
			if (first == "disablequant")
				as->disableQuant = convertStringBool(second);
			if (!useSparrow)
			{
				if (first == "up")
					up = secondSpaces;
				if (first == "left")
					left = secondSpaces;
				if (first == "down")
					down = secondSpaces;
				if (first == "right")
					right = secondSpaces;
			}
			else
			{
				if (first == "up")
					as->upA = secondSpaces;
				if (first == "left")
					as->leftA = secondSpaces;
				if (first == "down")
					as->downA = secondSpaces;
				if (first == "right")
					as->rightA = secondSpaces;
			}
			if (first == "holdleft")
				as->holdLeft = secondSpaces;
			if (first == "holdendleft")
				as->holdEndLeft = secondSpaces;
			if (first == "holdright")
				as->holdRight = secondSpaces;
			if (first == "holdendright")
				as->holdEndRight = secondSpaces;
			if (first == "holddown")
				as->holdDown = secondSpaces;
			if (first == "holdenddown")
				as->holdEndDown = secondSpaces;
			if (first == "holdup")
				as->holdUp = secondSpaces;
			if (first == "holdendup")
				as->holdEndUp = secondSpaces;

			if (first == "receptorleft")
				as->receptorLeft = secondSpaces;
			if (first == "receptorlitleft")
				as->receptorLitLeft = secondSpaces;
			if (first == "receptorhitleft")
				as->receptorHitLeft = secondSpaces;

			if (first == "receptorright")
				as->receptorRight = secondSpaces;
			if (first == "receptorlitright")
				as->receptorLitRight = secondSpaces;
			if (first == "receptorhitright")
				as->receptorHitRight = secondSpaces;

			if (first == "receptordown")
				as->receptorDown = secondSpaces;
			if (first == "receptorlitdown")
				as->receptorLitDown = secondSpaces;
			if (first == "receptorhitdown")
				as->receptorHitDown = secondSpaces;

			if (first == "receptorup")
				as->receptorUp = secondSpaces;
			if (first == "receptorlitup")
				as->receptorLitUp = secondSpaces;
			if (first == "receptorhitup")
				as->receptorHitUp = secondSpaces;

			if (first == "hitreceptorscale")
				as->hitReceptorScale = std::stof(second);

			if (first == "usesparrow")
				useSparrow = convertStringBool(second);

			if (first == "offsetxreceptorhit")
				as->offsetXReceptorHit = std::stoi(second);

			if (first == "offsetyreceptorhit")
				as->offsetYReceptorHit = std::stoi(second);

			if (first == "sparrow")
				sparrow = secondSpaces;

			if (first == "path")
				as->skinpath = secondSpaces;
		}
		config.close();
	}
	as->name = Noteskin::type;

	if (!useSparrow)
	{

		if (!as->disableQuant)
		{
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
		}
		else
		{
			as->up = getAsset("assets/noteskin/" + Noteskin::type + "/" + up + ".png");
			as->left = getAsset("assets/noteskin/" + Noteskin::type + "/" + left + ".png");
			as->down = getAsset("assets/noteskin/" + Noteskin::type + "/" + down + ".png");
			as->right = getAsset("assets/noteskin/" + Noteskin::type + "/" + right + ".png");
			as->receptor = getAsset("assets/noteskin/" + Noteskin::type + "/Receptor.png");
			as->hold = getAsset("assets/noteskin/" + Noteskin::type + "/hold.png");
			as->holdend = getAsset("assets/noteskin/" + Noteskin::type + "/holdend.png");
			as->light = getAsset("assets/noteskin/" + Noteskin::type + "/lit.png");
		}
	}
	else
	{
		as->sparrowImg = getAsset("assets/noteskin/" + Noteskin::type + "/" + sparrow + ".png");
		as->sparrow = new AvgSparrow("assets/noteskin/" + Noteskin::type + "/" + sparrow + ".xml", as->sparrowImg->width, as->sparrowImg->height);
	}
	std::cout << "[NOTESKIN] skinpath=" << as->skinpath << std::endl;
	VM_END
	return as;
}

noteskin_asset* Noteskin::getNoteskin()
{
	VM_START
	if (type != Game::save->GetString("Noteskin"))
	{
		type = Game::save->GetString("Noteskin");
		std::string path = std::filesystem::current_path().string() + "/assets/noteskin/" + type;
		if (!std::filesystem::exists(path))
		{
			Game::save->SetString("Noteskin", "arrow");
			Game::save->Save();
			type = "arrow";
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
	VM_END
	return asset;
}
