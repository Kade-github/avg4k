#pragma once
#include "GameObject.h"
#include "Receptor.h"

namespace Average4k::Objects::Gameplay {
	class Playfield : public AvgEngine::Base::GameObject
	{
	public:
		float time;
		float beat;

		float stops = 0;
		float stopEnd = 0;

		std::vector<int> keybinds = {};
		AvgEngine::OpenGL::Texture* arrowSpritesheet;
		AvgEngine::OpenGL::Texture* receptorSpritesheet;
		AvgEngine::OpenGL::Texture* holdSpritesheet;
		std::vector<Receptor*> receptors;
		Average4k::Chart::Difficulty* diff;
		Playfield(int _x, int _y, Average4k::Chart::Difficulty* d);
		~Playfield() {
			delete arrowSpritesheet;
			delete receptorSpritesheet;
			delete holdSpritesheet;
		}
		void draw();

		void keyPress(int key)
		{
			for(int i = 0; i < keybinds.size(); i++)
			{
				if (key == keybinds[i])
					receptors[i]->hit();
			}
		}

		void keyRelease(int key)
		{
			for (int i = 0; i < keybinds.size(); i++)
			{
				if (key == keybinds[i])
					receptors[i]->release();
			}
		}
	};
}