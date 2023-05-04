#pragma once
#include "includes.h"
#include "LuaChart.h"
#include "Pack.h"
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

		Average4k::Chart::Pack* base = NULL;

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