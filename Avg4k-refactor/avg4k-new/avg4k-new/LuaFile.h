#pragma once
#include "includes.h"

#include "LuaSprite.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>

namespace Average4k::Lua
{
	class LuaFile {
	public:
		sol::state* lua;

		std::string _path;

		bool good = false;

		LuaFile(std::string path)
		{
			_path = path;
			Reload();
		}

		void Launch();

		virtual void Load()
		{
			Function("create", "");
		}

		virtual void Reload()
		{
			Launch();
		}

		virtual void Function(std::string func, std::string args)
		{
			sol::protected_function f = (*lua)[func];

			if (!f.valid())
				return;

			sol::function_result x = f(args);
			if (!x.valid()) {
				sol::error errorstring = x;
				AvgEngine::Logging::writeLog("[Lua] Lua Error while running " + func + "!\n" + std::string(errorstring.what()));
			}
		}
	};
}