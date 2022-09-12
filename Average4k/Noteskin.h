#pragma once
#include "includes.h"
#include "Texture.h"
#include "AvgFrame.h"

class noteskin_asset {
public:
	Texture* fourth;
	Texture* eighth;
	Texture* twelfth;
	Texture* sixteenth;
	Texture* thirty2nd; // how do you actually write this??
	Texture* none;
	Texture* hold;
	Texture* receptor;
	Texture* holdend;
	Texture* light;

	Texture* left;
	Texture* down;
	Texture* up;
	Texture* right;

	Texture* sparrowImg;

	AvgSparrow* sparrow;

	std::string leftA;
	std::string rightA;
	std::string upA;
	std::string downA;

	std::string holdLeft;
	std::string holdEndLeft;

	std::string holdRight;
	std::string holdEndRight;

	std::string holdDown;
	std::string holdEndDown;

	std::string holdUp;
	std::string holdEndUp;

	std::string receptorLeft;
	std::string receptorLitLeft;
	std::string receptorHitLeft;
	std::string receptorUp;
	std::string receptorLitUp;
	std::string receptorHitUp;
	std::string receptorRight;
	std::string receptorLitRight;
	std::string receptorHitRight;
	std::string receptorDown;
	std::string receptorLitDown;
	std::string receptorHitDown;

	float hitReceptorScale = 1;

	int offsetXReceptorHit = 0;
	int offsetYReceptorHit = 0;

	bool disableQuant = false;
	bool rotate = true;
	bool bounce = true;
	bool shrink = true;
	std::string name = "arrow";
	std::string skinpath = "default";
};

class Noteskin
{
	public:

		static std::string type;
		static noteskin_asset* asset;
		static void resetNoteskin(noteskin_asset* as);
		static noteskin_asset* getNoteskin();
		static noteskin_asset* getNoteskin(std::string ty);

		static Texture* getGameplayElement(noteskin_asset* as, std::string element)
		{
			VM_START
			if (as->skinpath == "default")
				return Texture::createWithImage("assets/skinDefaults/Gameplay/" + element);
			VM_END
			return Texture::createWithImage("assets/noteskin/" + as->name + "/" + as->skinpath + "/Gameplay/" + element);
		}
		static Texture* getMenuElement(noteskin_asset* as, std::string element)
		{
			VM_START
			if (as->skinpath == "default")
				return Texture::createWithImage("assets/skinDefaults/Menu/" + element);
			VM_END
			return Texture::createWithImage("assets/noteskin/" + as->name + "/" + as->skinpath + "/Menu/" + element);
		}

		static Texture* getStartElement(noteskin_asset* as, std::string element)
		{
			VM_START
			if (as->skinpath == "default")
				return Texture::createWithImage("assets/skinDefaults/Start/" + element);
			VM_END
			return Texture::createWithImage("assets/noteskin/" + as->name + "/" + as->skinpath + "/Start/" + element);
		}

		static std::string getMusicElement(noteskin_asset* as, std::string element)
		{
			VM_START
			if (as->skinpath == "default")
				return "assets/skinDefaults/Music/" + element;
			VM_END
			return "assets/noteskin/" + as->name + "/" + as->skinpath + "/Music/" + element;
		}
};

