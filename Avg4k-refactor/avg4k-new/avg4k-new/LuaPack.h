#pragma once
#include "includes.h"
#include "LuaChart.h"
namespace Average4k::Lua::Menu
{
	struct pack
	{
	public:
		std::string name;
		std::string banner;

		bool showName;

		std::vector<chart> files;

		pack()
		{
			name = "";
			banner = "";
			showName = true;
			files = {};
		}
	};
}