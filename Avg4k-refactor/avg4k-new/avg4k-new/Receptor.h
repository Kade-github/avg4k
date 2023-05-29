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

		void draw() override {
			frame = 0;
			fps = 0;
			src.x = frame * frameWidth;
			src.y = 0;
			src.w = frameWidth;
			src.h = frameHeight;

			if (frame == 1)
				transform.scale = 0.85f;
			else
				transform.scale = 1.0f;

			AvgEngine::Base::Sprite::draw();
		}

		void hit() {
			frame = 1;
		}
	};
}