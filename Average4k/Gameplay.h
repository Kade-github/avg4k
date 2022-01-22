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
#include "SoundManager.h"
#include <SDL_image.h>
#include "ReceptorObject.h"
struct gameplayControl {
	int lane;
	SDL_Keycode code;
};

struct leaderboardSpot {
	SDL_FRect rect;
	Text* t;
	AvgRect* avgRect;
	PlayerScore score;

	friend bool operator==(const leaderboardSpot& lhs, const leaderboardSpot& rhs)
	{
		return lhs.score.SteamID64 == rhs.score.SteamID64;
	}
};

class Gameplay :
	public Menu
{
	public:
		std::vector<gameplayControl> controls;

		std::vector<ReceptorObject*> receptors;

		std::vector<note> notesToPlay;

		std::vector<leaderboardSpot> leaderboard;

		AvgSprite* background;

		static float rate;

		float scaleTime;

		virtual void create();

		int noteId;

		double songLength = 0;

		bool ended = false;

		bpmSegment curSeg;
		double beat;
		bool play = false;

		std::vector<NoteObject*> spawnedNotes;

		std::vector<float> clapped;

		float scaleStart = 0;

		int lastbeat = 0;
		float drop = 0;

		bool keys[4];
		bool holding[4];
		float startTime = 0;

		int combo = 0;

		double accuracy = 0;
		double notesPlayed = 0;
		double notesHit = 0;

		bool botplay = false;

		bool debug = false;

		std::vector<AvgGroup*> colGroups;

		int Marvelous = 0;
		int Perfect = 0;
		int Great = 0;
		int Eh = 0;
		int Yikes = 0;
		int Misses = 0;

		AvgRect* songPosOutline;
		AvgRect* songPosBar;

		float heldEnd[4];

		float positionInSong = 0;

		Text* songLengthText;

		Text* positionAndBeats;

		Text* Judgement;
		Text* Combo;

		Text* Accuracy;

		Text* Mrv;
		Text* Prf;
		Text* God;
		Text* Ehh;
		Text* Yke;

		bool downscroll;



		void cleanUp();
		
		Channel* song;
		Channel* clap;

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

