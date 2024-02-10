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

namespace Average4k::Screens::Menu
{
	class Gameplay : public AvgEngine::Base::Menu
	{
		Average4k::Data::SaveData* save;
		std::wstring _path;
		int _diff;

		AvgEngine::OpenGL::Texture* _noteskinSheet;
	public:
		int receptorColumn = 0;
		int receptorRow = 0;

		Average4k::Api::AvgLuaFile* lua;
		Average4k::Data::ChartFile chart;

		Average4k::Objects::RenderTexture* hud;

		AvgEngine::Base::Sprite* hud_spr;

		int noteWidth = 64;
		int noteHeight = 64;

		AvgEngine::Base::Sprite* background;

		std::vector<AvgEngine::Base::Sprite*> receptors;
		std::vector<Average4k::Objects::BaseNote*> notes;

		std::string comboTag = "";
		std::string judgementTextTag = "";
		std::map<std::string, std::string> judgementTags = {};

		Gameplay(std::wstring chart_path, int diff)
		{
			_path = chart_path;
			_diff = diff - 1; // - 1 cuz lua
		}

		~Gameplay()
		{
			delete hud;
			delete playfield;
		}

		void loadChart();
		void loadBackground();
		void loadPlayfield();

		void start();

		void load() override;
		void draw() override;
	};
}

#endif