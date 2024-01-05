/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_MENU_H
#define AVG4K_LUA_MENU_H

#pragma once

#include "../AvgLuaFile.h"
#include <AvgEngine/Base/Menu.h>

namespace Average4k::Api
{
	class LuaAvgMenu
	{
	public:
		AvgEngine::Base::Menu* _m = nullptr;

		LuaAvgMenu(AvgEngine::Base::Menu* m)
		{
			_m = m;
		}

		void addObject(sol::table o)
		{
			_m->addObject(Average4k::Api::AvgLuaFile::objects[o["id"]]);
		}

		void removeObject(sol::table o)
		{
			_m->removeObject(Average4k::Api::AvgLuaFile::objects[o["id"]]);
		}

		static void registerLua(sol::state& state)
		{
			state.new_usertype<LuaAvgMenu>("Menu",
							sol::constructors<LuaAvgMenu(AvgEngine::Base::Menu*)>(),
							"addObject", &LuaAvgMenu::addObject,
							"removeObject", &LuaAvgMenu::removeObject
						);
			
		}

	};

}

#endif