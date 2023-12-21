/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_MAINMENU_H
#define AVG4K_MAINMENU_H

#pragma once
#include <AvgEngine/Base/Menu.h>

namespace Average4k::Screens::Menu
{
	class MainMenu : public AvgEngine::Base::Menu
	{
		void load() override;
		void draw() override;
	};
}

#endif