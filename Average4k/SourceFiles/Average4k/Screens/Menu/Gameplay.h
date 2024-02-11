/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_GAMEPLAY_H
#define AVG4K_GAMEPLAY_H

#pragma once
#include <AvgEngine/Base/Menu.h>
#include "../../Data/Chart/Providers/StepFile.h"
#include "../../A4kGame.h"
#include "../../Api/AvgLuaFile.h"

#include "../../Objects/RenderTexture.h"
#include "../../Objects/Notes/Hold.h"
#include "../../Objects/UnicodeText.h"

#include <AvgEngine/External/Bass/BASS.h>

namespace Average4k::Screens::Menu
{
	class Gameplay : public AvgEngine::Base::Menu
	{
		Average4k::Data::SaveData* save;
		std::wstring _path;
		int _diff;

		AvgEngine::OpenGL::Texture* _noteskinSheet;
	public:
		sol::protected_function setupNote;
		sol::protected_function overlayUpdate;


		AvgEngine::Audio::Channel* channel;

		Average4k::Api::AvgLuaFile* lua;
		Average4k::Data::ChartFile chart;

		Average4k::Objects::RenderTexture* hud;
		Average4k::Objects::RenderTexture* playfield;

		AvgEngine::Base::Sprite* hud_spr;
		AvgEngine::Base::Sprite* playfield_spr;

		Average4k::Objects::UnicodeText* comboText;
		Average4k::Objects::UnicodeText* judgementText;
		Average4k::Objects::UnicodeText* accuracyText;

		bool stop = false;

		float noteScale = 1;
		float noteSpace = 1.1;
		float noteWidth = 64;
		float noteHeight = 64;

		float wScale = 1;
		float hScale = 1;

		AvgEngine::Base::Sprite* background;

		Average4k::Objects::BaseNote* holds[4] = { nullptr, nullptr, nullptr, nullptr };
		std::vector<AvgEngine::Base::Sprite*> receptors;
		std::vector<Average4k::Objects::BaseNote*> notes;

		std::vector<Average4k::Data::Chart::Note> cNotes;

		std::string comboTag = "";
		std::string judgementTextTag = "";
		std::string accuracyTag = "";

		std::map<std::string, std::string> judgementTags = {};

		int combo = 0;
		int hitNotes = 0;
		int totalNotes = 0;

		float scrollModifier = 1.0f;

		float _lastStretch = 1.0f;
		float stretch = 1.0f;

		float currentBeat = 0;
		float currentTime = 0;

		Gameplay(std::wstring chart_path, int diff)
		{
			_path = chart_path;
			_diff = diff - 1; // - 1 cuz lua
		}


		void leave();

		void loadAudio();
		void loadChart();
		void loadBackground();
		void loadPlayfield();

		void start();

		void load() override;
		void draw() override;

		int noteIndex = 0;

		void spawnNotes();
		void updateNotes();
	};
}

#endif