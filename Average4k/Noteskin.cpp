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
	if (!as->sparrow)
	{
		if (as->disableQuant)
		{
			delete as->left;
			delete as->up;
			delete as->down;
			delete as->right;
		}
		else
		{
			delete as->fourth;
			delete as->eighth;
			delete as->twelfth;
			delete as->sixteenth;
			delete as->thirty2nd;
			delete as->sixtyfourth;
		}

		delete as->receptor;
		delete as->light;
		delete as->hold;
		delete as->holdend;
	}
	else
		delete as->sparrowImg;

	delete as->judge_0;
	delete as->judge_1;
	delete as->judge_2;
	delete as->judge_3;
	delete as->judge_4;
	delete as->judge_5;
	delete as->num_0;
	delete as->num_1;
	delete as->num_2;
	delete as->num_3;
	delete as->num_4;
	delete as->num_5;
	delete as->num_6;
	delete as->num_7;
	delete as->num_8;
	delete as->num_9;
	delete as->num_period;
	delete as->num_perc;

	delete as;

	asset = NULL;
}


bool convertStringBool(std::string text) {
	std::transform(text.begin(), text.end(), text.begin(), Helpers::asciitolower);
	return text == "false" ? false : true;
}

noteskin_asset* loadSkin(noteskin_asset* as, std::string typ, bool appendPath = true) {
	VM_START
	as = new noteskin_asset();
	Noteskin::type = typ;

	std::string left;
	std::string right;
	std::string up;
	std::string down;


	std::string sparrow;

	std::string pat = "";

	if (appendPath)
		pat = "assets/noteskin/";

	bool useSparrow = false;
	std::ifstream config(pat + Noteskin::type + "/config.skin");
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

			if (first == "soundPath")
				as->soundpath = secondSpaces;
		}
		config.close();
	}
	as->name = Noteskin::type;

	if (!useSparrow)
	{

		if (!as->disableQuant)
		{
			as->fourth = getAsset(pat +  Noteskin::type + "/4th.png");
			as->eighth = getAsset(pat + Noteskin::type + "/8th.png");
			as->twelfth = getAsset(pat + Noteskin::type + "/12th.png");
			as->sixteenth = getAsset(pat + Noteskin::type + "/16th.png");
			as->thirty2nd = getAsset(pat + Noteskin::type + "/32nd.png");
			as->sixtyfourth = getAsset(pat + Noteskin::type + "/64th.png");
			as->none = getAsset(pat + Noteskin::type + "/idfk.png");
			as->receptor = getAsset(pat + Noteskin::type + "/Receptor.png");
			as->hold = getAsset(pat + Noteskin::type + "/hold.png");
			as->holdend = getAsset(pat + Noteskin::type + "/holdend.png");
			as->light = getAsset(pat + Noteskin::type + "/lit.png");
		}
		else
		{
			as->up = getAsset(pat + Noteskin::type + "/" + up + ".png");
			as->left = getAsset(pat + Noteskin::type + "/" + left + ".png");
			as->down = getAsset(pat + Noteskin::type + "/" + down + ".png");
			as->right = getAsset(pat + Noteskin::type + "/" + right + ".png");
			as->receptor = getAsset(pat + Noteskin::type + "/Receptor.png");
			as->hold = getAsset(pat + Noteskin::type + "/hold.png");
			as->holdend = getAsset(pat + Noteskin::type + "/holdend.png");
			as->light = getAsset(pat + Noteskin::type + "/lit.png");
		}
	}
	else
	{
		as->sparrowImg = getAsset(pat + Noteskin::type + "/" + sparrow + ".png");
		as->sparrow = new AvgSparrow(pat + Noteskin::type + "/" + sparrow + ".xml", as->sparrowImg->width, as->sparrowImg->height);
	}

	std::string path = pat + Noteskin::type;

	if (!Helpers::file_exists(path + "/judgements")) // default to arrow
		path =  "assets/noteskin/arrow";

	as->judge_0 = getAsset(path + "/judgements/judge_0.png");
	as->judge_1 = getAsset(path + "/judgements/judge_1.png");
	as->judge_2 = getAsset(path + "/judgements/judge_2.png");
	as->judge_3 = getAsset(path + "/judgements/judge_3.png");
	as->judge_4 = getAsset(path + "/judgements/judge_4.png");
	as->judge_5 = getAsset(path + "/judgements/judge_5.png");
	as->judge_bot = getAsset(path + "/judgements/judge_bot.png");
	as->num_0 = getAsset(path + "/judgements/num_0.png");
	as->num_1 = getAsset(path + "/judgements/num_1.png");
	as->num_2 = getAsset(path + "/judgements/num_2.png");
	as->num_3 = getAsset(path + "/judgements/num_3.png");
	as->num_4 = getAsset(path + "/judgements/num_4.png");
	as->num_5 = getAsset(path + "/judgements/num_5.png");
	as->num_6 = getAsset(path + "/judgements/num_6.png");
	as->num_7 = getAsset(path + "/judgements/num_7.png");
	as->num_8 = getAsset(path + "/judgements/num_8.png");
	as->num_9 = getAsset(path + "/judgements/num_9.png");
	as->num_period = getAsset(path + "/judgements/period.png");
	as->num_perc = getAsset(path + "/judgements/percentage.png");

	as->fontMap['0'] = as->num_0;
	as->fontMap['1'] = as->num_1;
	as->fontMap['2'] = as->num_2;
	as->fontMap['3'] = as->num_3;
	as->fontMap['4'] = as->num_4;
	as->fontMap['5'] = as->num_5;
	as->fontMap['6'] = as->num_6;
	as->fontMap['7'] = as->num_7;
	as->fontMap['8'] = as->num_8;
	as->fontMap['9'] = as->num_9;
	as->fontMap['.'] = as->num_period;
	as->fontMap[','] = as->num_period;
	as->fontMap['%'] = as->num_perc;

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

		std::cout << "creating noteskin " << type << std::endl;
		asset = loadSkin(asset, type);
		std::cout << "done noteskin" << std::endl;
	}
	else if (!asset)
	{
		std::cout << "creating noteskin " << type << std::endl;
		asset = loadSkin(asset, type);
		std::cout << "done noteskin" << std::endl;
	}
	VM_END
	return asset;
}

noteskin_asset* Noteskin::getNoteskin(std::string ty)
{
	VM_START
		std::cout << "creating noteskin " << ty << std::endl;
		resetNoteskin(Game::noteskin);
		asset = loadSkin(asset, ty, false);
		std::cout << "done noteskin" << std::endl;
	VM_END
		return asset;
}
