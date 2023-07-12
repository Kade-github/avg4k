#pragma once
#include "ChartFile.h"
#include "SpriteAnimated.h"


namespace Average4k::Objects::Gameplay {

	enum Judgement {
		Judgement_Marvelous = 0,
		Judgement_Perfect = 1,
		Judgement_Great = 2,
		Judgement_Good = 3,
		Judgement_Bad = 4,
		Judgement_Miss = 5,
		Judgement_Botplay = 6,
		Judgement_None = -1
	};

	class Note : public Average4k::External::Spritesheet::AnimatedSprite
	{
	public:
		float sTime;
		float sBeat;
		float time;
		float beat;

		bool downscroll = false;
		
		AvgEngine::OpenGL::Texture* holdTexture = NULL;

		Chart::NoteType type = Chart::NoteType::NoteType_Tap;
		float lengthInBeats = 0;
		int lane = 0;
		float stopHolding = 0;
		bool holding = false;
		float holdTimer = 1;
		bool holdJudged = false;
		bool judged = false;
		Judgement judge = Judgement_None;
		bool useXmod = true;
		float cmod = 0;
		float xmod = 1;
		float rate = 1;

		float noteSize = 0;
		bool holdDropped = false;
		void calculateJudge(float diff);

		float calculateY(bool xmodd, float diff)
		{
			if (!xmodd)
			{
				float bps = (cmod / 60);
				if (downscroll)
				{
					bps = (-cmod / 60);
					return ((-bps) *diff) * (64 * noteSize);
				}
				return ((bps) * diff) * (64 * noteSize);
			}
			else
			{
				if (downscroll)
					return (diff * ((64 * noteSize) * -xmod));
				return (diff * ((64 * noteSize) * xmod));
			}
		}

		Note(int _x, int _y, AvgEngine::OpenGL::Texture* t) : Average4k::External::Spritesheet::AnimatedSprite(_x, _y, t)
		{

		}

		void hit();
		void release()
		{
			holding = false;
			stopHolding = sTime;

			float end = beat + lengthInBeats;

			if (std::abs(end - sBeat) < 0.15f)
			{
				holdJudged = true;
			}
		}

		void draw() override;
	};
}