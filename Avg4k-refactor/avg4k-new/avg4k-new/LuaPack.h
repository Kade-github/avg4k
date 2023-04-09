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
		std::string path;

		bool showName;

		std::vector<chart> files;

		pack()
		{
			name = "";
			banner = "";
			path = "";
			showName = true;
			files = {};
		}
	};
}