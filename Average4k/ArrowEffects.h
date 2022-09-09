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
		float opac = 1;
		float holdSkewStart = 0;
		float holdSkewEnd = 0;
		float whiteV = 0;
		float cmod;
	};

	static float SplineDensity;

	static bool ShowSplines;

	static float drawBeats, SplineAlpha;

	static float drunk, tipsy, dizzy, amovex, amovey, aconfusion, wave;

	static std::map<int, float> stealthWhite;
	static std::map<int, float> stealthOpacity;
	static std::map<int, float> stealthReceptorOpacity;
	static std::map<int, float> reverse;
	static std::map<int, float> confusion;
	static std::map<int, float> movex;
	static std::map<int, float> movey;
	static std::map<int, float> drunkCol;
	static std::map<int, float> tipsyCol;
	static std::map<int, float> waveCol;

	static void drawLine(float defX, float targetY, int col, float beat, Chart currentChart);

	static void resetEffects();

	static Arrow ArrowEff(float cmod, int col, float pos);

	static Arrow finishEffects(float defX, float defY, int col, float curTime);
	static Arrow finishEffects(float defX, float defY, float cmod, int col, float holdTime, float curTime, float modY);
};