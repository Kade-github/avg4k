#pragma once
#include "includes.h"
#include "Game.h"
#define ARROW_SIZE 64

struct Spline {
	float x, y;

	bool goToReceptor = false;
	bool isFake = false;

	float beatAway = 0;

	inline bool operator() (const Spline& struct1, const Spline& struct2)
	{
		return (struct1.beatAway > struct2.beatAway);
	}
};

class ArrowEffects
{
public:
	struct Arrow {
		float x, y;
		float rot;
		float mini = 0;
		float opac = 1;
		float holdSkewStart = 0;
		float holdSkewEnd = 0;
		float whiteV = 0;
		float cmod;
	};

	static std::map<int, std::vector<Spline>> splines;


	static float SplineDensity;

	static bool ShowSplines;

	static float drawBeats, SplineAlpha;

	static float drunk, tipsy, dizzy, amovex, amovey, aconfusion, wave, mini;

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
	static std::map<int, float> miniCol;

	static void drawLine(float defX, float targetY, int col, float beat, Chart currentChart);

	static void resetEffects();

	static Arrow ArrowEff(float cmod, int col, float pos);

	static Arrow addSplines(float defX, float defY, Arrow aEff, float diff, int col, float curBeat, float targetBeat);

	static Arrow finishEffects(float defX, float defY, int col, float curTime);
	static Arrow finishEffects(float defX, float defY, float cmod, int col, float holdTime, float curTime, float modY, float noteBeat, float curBeat);
};