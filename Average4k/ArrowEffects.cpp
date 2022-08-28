#include "ArrowEffects.h"

float ArrowEffects::drunk = 0;
float ArrowEffects::tipsy = 0;
float ArrowEffects::adrunk = 0;
float ArrowEffects::atipsy = 0;


ArrowEffects::Arrow ArrowEffects::ArrowEff(float cmod, int col, float pos)
{
	Arrow a;
	a.x = 0;
	a.y = 0;
	a.rot = 0;

	if (drunk != 0)
		a.x += drunk * (std::cos(pos * 0.001 + col * (0.2) + cmod * (10) / 720) * (ARROW_SIZE / 2));
	if (tipsy != 0)
		a.y += tipsy * (std::cos(pos * 0.001 * (1.2) + col * (2.0) + 1 * (0.2)) * (ARROW_SIZE * .4));

	return a;
}

ArrowEffects::Arrow ArrowEffects::finishEffects(float defX, float defY, int col, float curTime)
{
	Arrow a;

	Arrow aEff = ArrowEff(0, col, curTime);

	a.x = defX + aEff.x;
	a.y = defY + aEff.y;
	a.rot = aEff.rot;

	return a;
}


ArrowEffects::Arrow ArrowEffects::finishEffects(float defX, float defY, float cmod, int col, float holdTime, float curTime)
{
	float yPos = defY + cmod;

	Arrow aEff = ArrowEff(yPos - defY, col, curTime);
	Arrow a;
	a.x = defX + aEff.x;
	a.y = yPos + aEff.y;
	return a;
}