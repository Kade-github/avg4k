/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_MAINMENU_H
#define AVG4K_MAINMENU_H

#pragma once
#include "../../Api/AvgLuaFile.h"
#include "../../Api/Stubs/LuaMenu.h"

namespace Average4k::Screens::Menu
{
	class MainMenu : public AvgEngine::Base::Menu
	{
	public:
		Average4k::Api::AvgLuaFile* lua;
		void createFile(std::string path, bool reset = true);
		void load() override;
		void draw() override;

		~MainMenu()
		{
			delete lua;
		}
	};
}

#endif