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
	};

	static float drunk, tipsy, adrunk, atipsy;

	static Arrow ArrowEff(float cmod, int col, float pos);

	static Arrow finishEffects(float defX, float defY, int col, float curTime);
	static Arrow finishEffects(float defX, float defY, float cmod, int col, float holdTime, float curTime);
};