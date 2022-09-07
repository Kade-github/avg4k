#include "ArrowEffects.h"

float ArrowEffects::drunk = 0;
float ArrowEffects::tipsy = 0;
float ArrowEffects::dizzy = 0;
float ArrowEffects::amovex = 0;
float ArrowEffects::amovey = 0;
float ArrowEffects::aconfusion = 0;
float ArrowEffects::drawBeats = 16;

std::map<int, float> ArrowEffects::stealthWhite = { {0,0}, {1,0}, {2,0}, {3,0} };
std::map<int, float> ArrowEffects::stealthReceptorOpacity = { {0,1}, {1,1}, {2,1}, {3,1} };
std::map<int, float> ArrowEffects::stealthOpacity = { {0,1}, {1,1}, {2,1}, {3,1} };
std::map<int, float> ArrowEffects::reverse = { {0,0}, {1,0}, {2,0}, {3,0} };
std::map<int, float> ArrowEffects::confusion = { {0,0}, {1,0}, {2,0}, {3,0} };
std::map<int, float> ArrowEffects::movex = { {0,0}, {1,0}, {2,0}, {3,0} };
std::map<int, float> ArrowEffects::movey = { {0,0}, {1,0}, {2,0}, {3,0} };

ArrowEffects::Arrow ArrowEffects::ArrowEff(float ydiff, int col, float pos)
{
	Arrow a;
	a.x = 0;
	a.y = 0;
	a.rot = 0;
	a.cmod = Game::instance->save->GetDouble("Scrollspeed");

	if (confusion[col] != 0)
		a.rot += confusion[col];

	if (dizzy != 0)
		a.rot += dizzy * ydiff;

	if (aconfusion != 0)
		a.rot += aconfusion;

	if (stealthOpacity[col] != 1)
		a.opac = stealthOpacity[col];

	if (stealthWhite[col] != 0)
	{
		a.whiteV = stealthWhite[col];
	}

	if (drunk != 0)
		a.x += drunk * (std::cos(pos * 0.001 + col * (0.2) + ydiff * (10) / 720) * (ARROW_SIZE / 2));
	if (tipsy != 0)
		a.y += tipsy * (std::cos(pos * 0.001 * (1.2) + col * (2.0) + 1 * (0.2)) * (ARROW_SIZE * .4));;

	if (movex[col] != 0)
		a.x += movex[col];

	if (movey[col] != 0)
		a.y += movey[col];

	if (amovex != 0)
		a.x += amovex;

	if (amovey != 0)
		a.y += amovey;

	if (reverse[col] != 0)
	{
		a.y += (500 - Game::save->GetDouble("Note Size")) * reverse[col];
		a.cmod = a.cmod * ((reverse[col] * -2) + 1);
	}

	return a;
}

void ArrowEffects::resetEffects()
{
	drawBeats = 16;
	ArrowEffects::stealthWhite = { {0,0}, {1,0}, {2,0}, {3,0} };
	ArrowEffects::stealthReceptorOpacity = { {0,1}, {1,1}, {2,1}, {3,1} };
	ArrowEffects::stealthOpacity = { {0,1}, {1,1}, {2,1}, {3,1} };
	ArrowEffects::reverse = { {0,0}, {1,0}, {2,0}, {3,0} };
	ArrowEffects::confusion = { {0,0}, {1,0}, {2,0}, {3,0} };
	ArrowEffects::movex = { {0,0}, {1,0}, {2,0}, {3,0} };
	ArrowEffects::movey = { {0,0}, {1,0}, {2,0}, {3,0} };
	ArrowEffects::drunk = 0;
	ArrowEffects::tipsy = 0;
	ArrowEffects::dizzy = 0;
	ArrowEffects::amovex = 0;
	ArrowEffects::amovey = 0;
	ArrowEffects::aconfusion = 0;
}

ArrowEffects::Arrow ArrowEffects::finishEffects(float defX, float defY, int col, float curTime)
{
	Arrow a;

	Arrow aEff = ArrowEff(0, col, curTime);

	if (stealthReceptorOpacity[col] != 1)
		a.opac = stealthReceptorOpacity[col];

	a.x = defX + aEff.x;
	a.y = defY + aEff.y;
	a.rot = aEff.rot;

	return a;
}

float calcCMod(float cmod, float diff)
{

	float bps = (cmod / 60);

	float noteOffset = (bps * (diff / 1000)) * (64 * Game::instance->save->GetDouble("Note Size"));

	return noteOffset;
}

ArrowEffects::Arrow ArrowEffects::finishEffects(float defX, float defY, float cmod, int col, float holdTime, float curTime, float diff)
{
	float yPos = defY + cmod;

	Arrow aEff = ArrowEff(yPos - defY, col, curTime);

	yPos = defY + calcCMod(aEff.cmod, diff);

	Arrow a;
	a.whiteV = aEff.whiteV;
	a.opac = aEff.opac;
	a.x = defX + aEff.x;
	a.y = yPos + aEff.y;
	a.rot = aEff.rot;
	return a;
}