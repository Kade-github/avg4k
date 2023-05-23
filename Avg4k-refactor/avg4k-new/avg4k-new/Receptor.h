#pragma once
#include "Note.h"
namespace Average4k::Objects::Gameplay {
	class Receptor : public Average4k::External::Spritesheet::AnimatedSprite
	{
	public:
		std::vector<Note*> notes{};

		Receptor(int _x, int _y, AvgEngine::OpenGL::Texture* receptor, AvgEngine::OpenGL::Texture* arrow) : Average4k::External::Spritesheet::AnimatedSprite(_x, _y, receptor)
		{
		}
	};
}