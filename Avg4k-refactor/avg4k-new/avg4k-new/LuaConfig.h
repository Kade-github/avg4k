#pragma once
#include "includes.h"
namespace Average4k::Lua::Base
{
	struct LuaConfigValue {
	public:
		std::string name;
		std::string value;
	};

	struct LuaConfig {
	public:
		std::vector<LuaConfigValue> values;
		LuaConfig(std::string path)
		{

		}
	};
}