#pragma once
#include "LuaGameObject.h"
#include "LuaTexture.h"
namespace Average4k::Lua::Base
{
	struct sprite : public gameObject {
	public:
		texture tex{};

		sprite(float x, float y, std::string path) : gameObject(x, y)
		{
			tex.tex = AvgEngine::OpenGL::Texture::createWithImage(path);
			tex.w = tex.tex->width;
			tex.h = tex.tex->height;
			tex.path = path;
		}

		void Destroy()
		{

		}
	};
}