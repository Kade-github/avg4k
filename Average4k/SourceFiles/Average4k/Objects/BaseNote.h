/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef AVG4K_BASE_NOTE_H
#define AVG4K_BASE_NOTE_H

#include <AvgEngine/Base/Sprite.h>
#include "../Data/Chart/Difficulty.h"
#include "../Data/Chart/ChartFile.h"

namespace Average4k::Objects
{
	class BaseNote : public AvgEngine::Base::Sprite
	{
	public:
		bool downscroll = false;
		float cmod = -1.0f;
		float xmod = -1.0f;
		float endBeat = 0;
		float endTime = 0;

		float currentBeat = 0;
		float currentTime = 0;
		float noteTime = 0;

		int sheet_row = 0;
		int sheet_col = 0;

		int sheet_end_row = 0;
		int sheet_end_col = 0;

		std::vector<int> overlays = {};

		float overlayAngle = 0;
		float overlayOpacity = 1;

		Average4k::Data::Chart::Note data;

		BaseNote(AvgEngine::OpenGL::Texture* spritesheet, Average4k::Data::Chart::Note _data, Average4k::Data::ChartFile* file, bool _downscroll = false, float _cmod = -1, float _xmod = -1) : AvgEngine::Base::Sprite(0,0, spritesheet)
		{
			downscroll = _downscroll;
			data = _data;
			cmod = _cmod;
			xmod = _xmod;
			noteTime = file->GetTimeFromBeat(data.beat);
		}

		void setPosition();

		void draw() override;
	};
}

#endif