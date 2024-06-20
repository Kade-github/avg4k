/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_ONLINE_H
#define AVG4K_F_ONLINE_H

#pragma once

#include <string>
#include "../../Multiplayer/Connection.h"
#include "../AvgLuaFile.h"

namespace Average4k::Api::Functions
{
	class FOnline {
	public:
		static void QueryLobbies();
		static sol::table GetLobbies();
	};
}

#endif