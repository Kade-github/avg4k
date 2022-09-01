#pragma once
#include "includes.h"
#include "Game.h"
#define ARROW_SIZE 64

class ArrowEffects
{
public:
	struct Arrow {
		float x, y;
		float rot;
		float holdSkewStart = 0;
		float holdSkewEnd = 0;
		float cmod;
	};

	static float drunk, tipsy, dizzy, amovex, amovey;

	static std::map<int, float> reverse;
	static std::map<int, float> confusion;
	static std::map<int, float> movex;
	static std::map<int, float> movey;

	static void resetEffects();

	static Arrow ArrowEff(float cmod, int col, float pos);

	static Arrow finishEffects(float defX, float defY, int col, float curTime);
	static Arrow finishEffects(float defX, float defY, float cmod, int col, float holdTime, float curTime, float modY);
};