/*
	Avg Dev Team
	2021-2024 Average4k
*/

#ifndef AVG4K_SKIN_H
#define AVG4K_SKIN_H

#pragma once

#include "../Data/Types/SkinData.h"
#include <AvgEngine/Base/Sprite.h>
#include <AvgEngine/Base/Text.h>

namespace Average4k
{
	class Skin {
	private:
		Data::Types::SkinData data;
		std::map<std::string, std::shared_ptr<AvgEngine::OpenGL::Texture>> textures;
	public:
		Skin(Data::Types::SkinData _data);
		Skin() {};
		~Skin();

		void ClearTextures();

		std::string GetPath(std::string p);

		std::shared_ptr<AvgEngine::Base::Sprite> CreateSprite(std::string texture, bool unique = false);
		std::shared_ptr<AvgEngine::Base::Text> CreateText(std::string font, int size);
	};
}

#endif