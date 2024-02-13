/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef AVG4K_HOLD_NOTE_H
#define AVG4K_HOLD_NOTE_H

#include "../BaseNote.h"

namespace Average4k::Objects {
	class HoldNote : public BaseNote {
	public:
		bool hasHeld = false;
		bool lastHold = false;
		float endHold = INT_MIN;
		bool holding = false;

		bool holdHasEnded = false;

		HoldNote(AvgEngine::OpenGL::Texture* spritesheet, Average4k::Data::Chart::Note _data, Average4k::Data::ChartFile* file, bool _downscroll = false, float _cmod = -1, float _xmod = -1) : BaseNote(spritesheet, _data, file, _downscroll, _cmod, _xmod) {}
		void draw() override;
	};
}

#endif