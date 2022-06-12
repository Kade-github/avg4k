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
		NoteObject();
		~NoteObject() {};
		note* connected;
		Text* debugText;

		int modId = 0;

		bool fboMode = true;
		bool drawCall = true;

		float calcCMod();

		Shader* customShader;

		Chart* currentChart;

		float time;

		bool selected = false;

		Object* connectedReceptor;

		float stopOffset;

		int holdHeight;

		float size;

		int holdsActive = 0;
		bool wasHit;
		bool clapped;

		float tailBeat = 0;

		bool debug = false;
		double beat = 0;
		float endTime = 0;
		float fboX = 0;
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

