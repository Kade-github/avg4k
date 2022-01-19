#pragma once
#include "includes.h"
#include "Texture.h"

struct noteskin_asset {
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
};

class Noteskin
{
	public:

		static std::string type;
		static noteskin_asset* asset;
		static void resetNoteskin(noteskin_asset* as);
		static noteskin_asset* getNoteskin();
};

