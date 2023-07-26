#pragma once
#include "GameObject.h"
#include "Receptor.h"

namespace Average4k::Objects::Gameplay {
	class Playfield : public AvgEngine::Base::GameObject
	{
	public:
		float time;
		float beat;

		bool judgeable = true;

		float stops = 0;
		float stopEnd = 0;
		bool downscroll = false;
		std::vector<int> keybinds = {};
		AvgEngine::OpenGL::Texture* arrowSpritesheet;
		AvgEngine::OpenGL::Texture* receptorSpritesheet;
		AvgEngine::OpenGL::Texture* holdSpritesheet;
		AvgEngine::OpenGL::Texture* expSpritesheet;
		std::vector<Receptor*> receptors;
		Average4k::Chart::Difficulty* diff;
		Playfield(int _x, int _y, Average4k::Chart::Difficulty* d, bool _downscroll);
		~Playfield() {
			delete arrowSpritesheet;
			delete receptorSpritesheet;
			delete holdSpritesheet;
			delete expSpritesheet;
			AvgEngine::Base::GameObject::~GameObject();
		}

		void createNotes();

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