#pragma once
#include "Note.h"
namespace Average4k::Objects::Gameplay {
	class Receptor : public Average4k::External::Spritesheet::AnimatedSprite
	{
	public:
		float time;
		float beat;

		bool downscroll = false;

		Average4k::External::Spritesheet::AnimatedSprite* explosion = NULL;

		int lane = 0;

		bool judgeable = true;

		float noteSize = 0;

		std::vector<Chart::Note> notes{};
		AvgEngine::OpenGL::Texture* arrowSpritesheet = NULL;
		AvgEngine::OpenGL::Texture* holdSpritesheet = NULL;

		float hitTimestamp = 0;

		Receptor(int _x, int _y, AvgEngine::OpenGL::Texture* receptor, AvgEngine::OpenGL::Texture* arrow, AvgEngine::OpenGL::Texture* exp) : Average4k::External::Spritesheet::AnimatedSprite(_x, _y, receptor)
		{
			Create(_x, _y, arrow, exp);
		}

		void Create(int _x, int _y, AvgEngine::OpenGL::Texture* arrow, AvgEngine::OpenGL::Texture* exp);

		void blowUp();

		void draw() override;

		void hit();

		void release();
	};
}