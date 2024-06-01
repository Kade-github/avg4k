/*
	Avg Dev Team
	2021-2024 Average4k
*/

#include "Skin.h"

Average4k::Skin::Skin(Data::Types::SkinData _data)
{
	data = _data;
}

Average4k::Skin::~Skin()
{
}

void Average4k::Skin::ClearTextures()
{
	AvgEngine::Fnt::Fnt::ClearCache();
	textures.clear();
}

std::string Average4k::Skin::GetPath(std::string p)
{
	return data.path + "/" + p;
}

std::shared_ptr<AvgEngine::Base::Sprite> Average4k::Skin::CreateSprite(std::string texture, bool unique)
{
	std::shared_ptr<AvgEngine::OpenGL::Texture> t = nullptr;

	if (!unique)
		for (auto& tex : textures)
		{
			if (tex.first == texture)
			{
				t = tex.second;
				break;
			}
		}
	if (t == NULL)
	{
		t = AvgEngine::OpenGL::Texture::createWithImage(GetPath("") + texture);
		if (!unique)
			textures[texture] = t;
	}
	std::shared_ptr<AvgEngine::Base::Sprite> s = std::make_shared<AvgEngine::Base::Sprite>(0, 0, t);
	if (!unique)
		s->dontDelete = true;
	return s;
}

std::shared_ptr<AvgEngine::Base::Text> Average4k::Skin::CreateText(std::string font, int size)
{
	std::shared_ptr<AvgEngine::Base::Text> t = std::make_shared<AvgEngine::Base::Text>(0,0, GetPath("/Fonts/"), font, "", size);
	return t;
}
