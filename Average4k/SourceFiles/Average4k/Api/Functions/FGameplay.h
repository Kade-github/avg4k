/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_GAMEPLAY_H
#define AVG4K_F_GAMEPLAY_H

#pragma once

#include <string>

namespace Average4k::Api::Functions
{
	class FGameplay {
	public:
		static void SetNoteSize(int width, int height);
		static void SetComboTag(std::string tag);
		static void SetJudgementTag(std::string judge, std::string tag);
		static void SetJudgementTextTag(std::string tag);
	};
}

#endif