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

	std::map<int, std::vector<Spline>> splines;


	float SplineDensity;

	bool ShowSplines = false;

	float drawBeats, SplineAlpha;

	float scrollSpeed;
	float noteSize;

	float drunk, tipsy, dizzy, amovex, amovey, aconfusion, wave, mini, rotz;

	std::map<int, float> drawSize;
	std::map<int, float> skewX;
	std::map<int, float> skewY;
	std::map<int, float> stealthWhite;
	std::map<int, float> stealthOpacity;
	std::map<int, float> stealthReceptorOpacity;
	std::map<int, float> reverse;
	std::map<int, float> confusion;
	std::map<int, float> movex;
	std::map<int, float> movey;
	std::map<int, float> drunkCol;
	std::map<int, float> tipsyCol;
	std::map<int, float> waveCol;
	std::map<int, float> miniCol;
	std::map<int, float> rotzCol;

	void drawLine(float defX, float targetY, int col, float beat, Chart currentChart);

	void resetEffects();

	Arrow ArrowEff(float cmod, int col, float pos);

	Arrow addSplines(float defX, float defY, Arrow aEff, float diff, int col, float curBeat, float targetBeat);

	Arrow finishEffects(float defX, float defY, int col, float curTime);
	std::vector<Arrow> finishEffects(float defX, float defY, float cmod, int col, float holdTime, float curTime, float modY, float noteBeat, float curBeat);
};