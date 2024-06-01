/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_MENU_H
#define AVG4K_LUA_MENU_H

#pragma once
#include <AvgEngine/Base/Menu.h>

#include "LuaObject.h"
#include "../AvgLuaFile.h"

#define SOL_USE_LUA_HPP
#include <sol/sol.hpp>

namespace Average4k::Api::Stubs
{

	class LuaMenu {
		std::shared_ptr<AvgEngine::Base::Menu> _base;
	public:
		LuaMenu()
		{
			_base = nullptr;
		}
		LuaMenu(std::shared_ptr<AvgEngine::Base::Menu> base)
		{
			_base = base;
		}

		/// <summary>
		/// Add an object to the menu
		/// </summary>
		/// <param name="obj">The object</param>
		void addObject(LuaObject obj)
		{
			if (obj._baseObject == nullptr)
			{
				AvgEngine::Logging::writeLog("[LuaMenu] You cannot add an object that has already been destroyed.");
				return;
			}
			_base->addObject(obj._baseObject);

			AvgLuaFile::objects.push_back(obj._baseObject);
		}

		/// <summary>
		/// Remove an object from the menu
		/// </summary>
		/// <param name="obj">The object</param>
		void removeObject(LuaObject obj)
		{
			_base->removeObject(obj._baseObject);

			AvgLuaFile::objects.erase(std::remove(AvgLuaFile::objects.begin(), AvgLuaFile::objects.end(), obj._baseObject), AvgLuaFile::objects.end());
		}

		/// <summary>
		/// Remove all objects from the menu
		/// </summary>
		void removeAll()
		{
			AvgLuaFile::objects.clear();
			_base->removeAllObjects();
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