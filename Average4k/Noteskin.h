#pragma once
#include "includes.h"
#include "Texture.h"

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
	bool rotate;
	bool bounce;
	bool shrink;
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

		static Texture* getGameplayElement(noteskin_asset* as, std::string element)
		{
			if (as->skinpath == "default")
				return Texture::createWithImage("assets/skinDefaults/Gameplay/" + element);
			return Texture::createWithImage("assets/noteskin/" + as->name + "/" + as->skinpath + "/Gameplay/" + element);
		}
		static Texture* getMenuElement(noteskin_asset* as, std::string element)
		{
			if (as->skinpath == "default")
				return Texture::createWithImage("assets/skinDefaults/Menu/" + element);
			return Texture::createWithImage("assets/noteskin/" + as->name + "/" + as->skinpath + "/Menu/" + element);
		}
};

