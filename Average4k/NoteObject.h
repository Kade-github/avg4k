#pragma once
#include "includes.h"
#include "SMFile.h"
#include "MainMenu.h"
#include "Chart.h"

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

class NoteObject : public Object
{
	public:
		NoteObject()  {
			setX(0);
			setY(0);
			w = 64;
			h = 64;
			Object::currentId++;
			id = Object::currentId;
		};
		~NoteObject() {};
		note* connected;
		Text* debugText;

		bool fboMode = true;

		Chart* currentChart;

		float time;

		Object* connectedReceptor;

		float stopOffset;

		int holdHeight;

		int holdsActive = 0;
		bool wasHit;
		bool clapped;

		float tailBeat = 0;

		bool debug = false;
		double beat = 0;
		float endTime = 0;
		double endBeat = 0;
		float rTime = 0;
		bool active = true;
		bool destroyed = false;
		int lane = 0;
		noteType type = noteType::Note_Normal;
		SDL_FRect rect;
		std::vector<holdTile> heldTilings;


		virtual void draw();
};

