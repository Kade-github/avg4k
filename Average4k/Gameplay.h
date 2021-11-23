#pragma once
#include "includes.h"
#include "NoteObject.h"
#include "Chart.h"
#include "Menu.h"
#include "MainMenu.h"
#include "Game.h"
#include "Noteskin.h"
#include "Text.h"
#include "bass.h"
#include "Judge.h"
#include <algorithm>
#include "bass_fx.h"


struct gameplayControl {
	int lane;
	SDL_Keycode code;
};

struct Receptor {
	SDL_FRect rect;
};

class Gameplay :
	public Menu
{
	public:

		std::vector<gameplayControl> controls;

		std::vector<Receptor> receptors;

		std::vector<note> notesToPlay;

		bpmSegment curSeg;
		double beat;
		bool play = false;

		std::vector<NoteObject*> spawnedNotes;

		std::vector<float> clapped;

		bool keys[3];

		float startTime = 0;

		int combo = 0;

		double accuracy = 0;
		double notesPlayed = 0;
		double notesHit = 0;

		bool botplay = false;

		int Marvelous = 0;
		int Perfect = 0;
		int Great = 0;
		int Eh = 0;
		int Yikes = 0;
		int Misses = 0;

		float heldEnd[3];

		float positionInSong = 0;

		Text* positionAndBeats;

		Text* Judgement;
		Text* Combo;
		Text* Accuracy;

		bool downscroll;

		unsigned long channel;
		unsigned long tempostream;

		static noteskin_asset* noteskin;

		Gameplay();

		void initControls();

		void removeNote(NoteObject* object);

		void updateAccuracy(double hitWorth);

		void miss(NoteObject* object);

		void update(Events::updateEvent event) override;
		void keyDown(SDL_KeyboardEvent event) override;
		void keyUp(SDL_KeyboardEvent ev) override;
};

