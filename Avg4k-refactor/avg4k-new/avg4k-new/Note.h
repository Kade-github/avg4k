#pragma once
#include "ChartFile.h"
#include "SpriteAnimated.h"


namespace Average4k::Objects::Gameplay {

	enum Judgement {
		Judgement_None,
		Judgement_Miss,
		Judgement_Bad,
		Judgement_Good,
		Judgement_Great,
		Judgement_Perfect
	};

	class Note : public Average4k::External::Spritesheet::AnimatedSprite
	{
	public:
		float sTime;
		float time;
		float beat;
		
		Chart::NoteType type = Chart::NoteType::NoteType_Tap;
		float lengthInBeats = 0;
		int lane = 0;

		bool judged = false;
		Judgement judge = Judgement_None;

		float cmod = 0;
		float xmod = 0;
		float rate = 1;

		float noteSize = 0;

		void calculateJudge(float diff);

		float calculateY(bool xmod, float diff)
		{
			if (!xmod)
			{
				return (((cmod / 60)) * (diff / 1000)) * (64 * noteSize);
			}
			else
			{
				return 0; // TODO
			}
		}

		Note(int _x, int _y, AvgEngine::OpenGL::Texture* t) : Average4k::External::Spritesheet::AnimatedSprite(_x, _y, t)
		{

		}

		void draw() override;
	};
}