#pragma once
#include "includes.h"
#include "Texture.h"
namespace Average4k::Lua::Base
{
	struct texture {
	public:
		AvgEngine::OpenGL::Texture* tex = NULL;

		int w = 0, h = 0;
		std::string path = "";

		texture() = default;
	};
}