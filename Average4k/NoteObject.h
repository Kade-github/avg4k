#pragma once
#include "includes.h"
#include "SMFile.h"
#include "MainMenu.h"

struct holdTile {
	float time;
	double beat;
	bool active;
	bool fucked;
	int index;
	SDL_FRect rect;

	bool operator<(const holdTile& a) const
	{
		return beat < a.beat;
	}
};

class NoteObject
{
	public:
		NoteObject();
		~NoteObject() {};

		Text* debugText;

		int holdHeight;

		int holdsActive = 0;
		bool wasHit;
		bool clapped;

		bool debug = false;
		double beat = 0;
		float endTime = 0;
		double endBeat = 0;
		float time = 0;
		bool active = true;
		bool destroyed = false;
		int lane = 0;
		noteType type = Note_Normal;
		SDL_FRect rect;
		std::vector<holdTile> heldTilings;


		void destroy();

		void draw(float position, double beat, SDL_FRect receptor, bool clipHold);
};

