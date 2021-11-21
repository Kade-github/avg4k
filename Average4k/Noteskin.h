#pragma once
#include "includes.h"

struct noteskin_asset {
	SDL_Texture* fourth;
	SDL_Texture* eighth;
	SDL_Texture* twelfth;
	SDL_Texture* sixteenth;
	SDL_Texture* thirty2nd; // how do you actually write this??
	SDL_Texture* none;
	SDL_Texture* hold;
	SDL_Texture* receptor;
};

class Noteskin
{
	public:

		static std::string type;

		static noteskin_asset* getNoteskin();
};

