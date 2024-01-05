/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_LUA_FILE_H
#define AVG4K_LUA_FILE_H

#pragma once

#define SOL_USE_LUA_HPP

#include <AvgEngine/Base/GameObject.h>

#include <sol/sol.hpp>

namespace Average4k::Api
{
	class AvgLuaFile
	{
	public:
		static std::vector<AvgEngine::Base::GameObject*> objects;

		AvgLuaFile() = default;
		AvgLuaFile(const std::string& path);
		~AvgLuaFile();

		void load(const std::string& path);
		void reload();

		void create();

		sol::state& getState();

	private:
		std::unique_ptr<sol::state> state;
		std::string path;
	};
}

#endif