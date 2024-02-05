/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_MENU_H
#define AVG4K_LUA_MENU_H

#pragma once
#include "LuaObject.h"
#include "AvgEngine/Base/Menu.h"

#define SOL_NO_EXCEPTIONS 1
#define SOL_USE_LUA_HPP
#include <sol/sol.hpp>

namespace Average4k::Api::Stubs
{

	class LuaMenu {
		AvgEngine::Base::Menu* _base;
	public:
		LuaMenu(AvgEngine::Base::Menu* base)
		{
			_base = base;
		}

		void addObject(LuaObject obj)
		{
			_base->addObject(obj._baseObject);

			AvgLuaFile::objects.push_back(obj._baseObject);
		}

		void removeObject(LuaObject obj)
		{
			_base->removeObject(obj._baseObject);
			AvgLuaFile::objects.push_back(obj._baseObject);
		}

		void removeAll()
		{
			_base->removeAll();
		}

		static void Register(sol::state& state)
		{
			state.new_usertype<LuaMenu>("Menu",
								"addObject", &LuaMenu::addObject,
								"removeObject", &LuaMenu::removeObject,
								"removeAll", &LuaMenu::removeAll
						);
		}

	};
}

#endif