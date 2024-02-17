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
		/// <summary>
		/// Get the current available skins
		/// </summary>
		/// <returns>The skins in a lua table</returns>
		static sol::table GetSkins();
		/// <summary>
		/// Get the current available noteskins
		/// </summary>
		/// <returns>The noteskins in a lua table</returns>
		static sol::table GetNoteskins();
	};
}

#endif