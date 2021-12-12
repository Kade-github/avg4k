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
#include <SDL_image.h>
#include "ReceptorObject.h"
#include "SparrowAtlas.h"
struct gameplayControl {
	int lane;
	SDL_Keycode code;
};

struct leaderboardSpot {
	SDL_FRect rect;
	Text* t;
	PlayerScore score;
};

class Gameplay :
	public Menu
{
	public:
		std::vector<gameplayControl> controls;

		std::vector<ReceptorObject*> receptors;

		SparrowAtlas* gf;
		SparrowAtlas* bf;

		std::vector<note> notesToPlay;

		std::vector<leaderboardSpot> leaderboard;

		SDL_Texture* background;

		static float rate;

		int noteId;

		double songLength = 0;

		bool ended = false;

		bpmSegment curSeg;
		double beat;
		bool play = false;

		std::vector<NoteObject*> spawnedNotes;

		std::vector<float> clapped;

		bool keys[4];

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

		float heldEnd[4];

		float positionInSong = 0;

		Text* positionAndBeats;

		Text* Judgement;
		Text* Combo;
		Text* Accuracy;

		bool downscroll;

		void cleanUp();

		unsigned long channel;
		unsigned long tempostream;

		unsigned long clap;

		static noteskin_asset* noteskin;

		Gameplay();

		void initControls();

		void removeNote(NoteObject* object);

		void updateAccuracy(double hitWorth);
		
		void miss(NoteObject* object);
		void onPacket(PacketType pt, char* data, int32_t length);
		void update(Events::updateEvent event) override;
		void keyDown(SDL_KeyboardEvent event) override;
		void keyUp(SDL_KeyboardEvent ev) override;
};

