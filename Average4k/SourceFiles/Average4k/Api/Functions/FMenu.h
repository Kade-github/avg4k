/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_F_MENU_H
#define AVG4K_F_MENU_H

#pragma once

#include <string>
#include "../Stubs/LuaMenu.h"

namespace Average4k::Api::Functions
{
	class FMenu {
	public:
		static Stubs::LuaMenu* CurrentMenu;

		static void SwitchTo(std::string path);
	};
}

#endif