#pragma once
#include "SpriteAnimated.h"
namespace Average4k::Objects::Gameplay {
	class Note : public Average4k::External::Spritesheet::AnimatedSprite
	{
	public:

		Note(int _x, int _y, AvgEngine::OpenGL::Texture* t) : Average4k::External::Spritesheet::AnimatedSprite(_x, _y, t)
		{
		}
	};
}