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
		static std::vector<std::shared_ptr<AvgEngine::Base::GameObject>> objects;

		AvgLuaFile() = default;
		AvgLuaFile(const std::string& path);

		void load(const std::string& path);
		void reload();

		void create();

		void reset();
		void registerTypes();

		sol::state& getState();

		std::string path;
		void luaPanic(sol::optional<std::string> maybe_msg);

	private:
		bool loaded = false;
		std::unique_ptr<sol::state> state;
		std::string _path;
	};
}

#endif