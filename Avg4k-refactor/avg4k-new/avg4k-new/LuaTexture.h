#pragma once
#include "includes.h"
#include "Texture.h"
namespace Average4k::Lua::Base
{
	struct texture {
	public:
		int id = -1;

		int w = 0, h = 0;
		std::string path = "";

		texture(std::string _p)
		{
			path = _p;
			id = -1;
			w = 0;
			h = 0;
		}
	};
}