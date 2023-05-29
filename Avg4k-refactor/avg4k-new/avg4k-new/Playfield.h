#pragma once
#include "GameObject.h"
#include "Receptor.h"
namespace Average4k::Objects::Gameplay {
	class Playfield : public AvgEngine::Base::GameObject
	{
	public:
		AvgEngine::OpenGL::Texture* arrowSpritesheet;
		AvgEngine::OpenGL::Texture* receptorSpritesheet;
		std::vector<Receptor*> receptors;

		Playfield(int _x, int _y);

		void keyPress(int key)
		{
			for (Receptor* r : receptors)
			{

			}
		}
	};
}