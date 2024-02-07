/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_SKINS_H
#define AVG4K_F_SKINS_H

#pragma once

#include "FCharts.h"

namespace Average4k::Api::Functions
{
	class FSkins {
	public:
		static sol::table GetSkins();
		static sol::table GetNoteskins();
	};
}

#endif