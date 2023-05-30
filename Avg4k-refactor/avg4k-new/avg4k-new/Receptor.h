#pragma once
#include "Note.h"
namespace Average4k::Objects::Gameplay {
	class Receptor : public Average4k::External::Spritesheet::AnimatedSprite
	{
	public:
		float time;
		float beat;

		int lane = 0;

		float noteSize = 0;

		std::vector<Chart::Note> notes{};

		AvgEngine::OpenGL::Texture* arrowSpritesheet;
		AvgEngine::OpenGL::Texture* holdSpritesheet;

		float hitTimestamp = 0;

		Receptor(int _x, int _y, AvgEngine::OpenGL::Texture* receptor, AvgEngine::OpenGL::Texture* arrow) : Average4k::External::Spritesheet::AnimatedSprite(_x, _y, receptor)
		{
			arrowSpritesheet = arrow;
		}

		void draw() override;

		void hit();

		void release();
	};
}