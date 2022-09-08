#include "ArrowEffects.h"

float ArrowEffects::drunk = 0;
float ArrowEffects::tipsy = 0;
float ArrowEffects::dizzy = 0;
float ArrowEffects::amovex = 0;
float ArrowEffects::amovey = 0;
float ArrowEffects::aconfusion = 0;
float ArrowEffects::drawBeats = 8;
float ArrowEffects::SplineAlpha = 0.75;
float ArrowEffects::SplineDensity = 0.05;

bool ArrowEffects::ShowSplines = false;

std::map<int, float> ArrowEffects::stealthWhite = { {0,0}, {1,0}, {2,0}, {3,0} };
std::map<int, float> ArrowEffects::stealthReceptorOpacity = { {0,1}, {1,1}, {2,1}, {3,1} };
std::map<int, float> ArrowEffects::stealthOpacity = { {0,1}, {1,1}, {2,1}, {3,1} };
std::map<int, float> ArrowEffects::reverse = { {0,0}, {1,0}, {2,0}, {3,0} };
std::map<int, float> ArrowEffects::confusion = { {0,0}, {1,0}, {2,0}, {3,0} };
std::map<int, float> ArrowEffects::movex = { {0,0}, {1,0}, {2,0}, {3,0} };
std::map<int, float> ArrowEffects::movey = { {0,0}, {1,0}, {2,0}, {3,0} };

float calcCMod(float cmod, float diff)
{

	float bps = (cmod / 60);

	float noteOffset = (bps * (diff / 1000)) * (64 * Game::instance->save->GetDouble("Note Size"));

	return noteOffset;
}


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
	drawBeats = 8;
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


void ArrowEffects::drawLine(float defX, float targetY, int col, float beat, Chart currentChart)
{
	bpmSegment segCur = currentChart.getSegmentFromBeat(beat);
	float timeCur = currentChart.getTimeFromBeat(beat, segCur);

	std::vector<GL_Vertex> points;

	int realI = 0;

	float lastBottomX = -1;
	float lastBottomY = -1;

	float nextY = 0;
	float nextX = 0;

	GL_Vertex lastBL;
	lastBL.a = -1;
	GL_Vertex lastBR;
	lastBR.a = -1;
	GL_Vertex lastTR;
	lastTR.a = -1;
	for (double i = -drawBeats; i < drawBeats; i += SplineDensity)
	{
		float rbeat = beat + i;
		bpmSegment seg = currentChart.getSegmentFromBeat(rbeat);
		float time = currentChart.getTimeFromBeat(rbeat, seg);

		float diff = (time - timeCur);

		float cmod = calcCMod(Game::save->GetDouble("Scrollspeed"), diff);

		bool downscroll = false;


		float defY = targetY - cmod;

		Arrow aEff = ArrowEff(defY, col, time);
		
		if (aEff.cmod < 0)
			downscroll = true;

		float yPos = targetY + calcCMod(aEff.cmod, diff);

		float x = (defX + (32 * Game::save->GetDouble("Note Size")) - 2) + aEff.x;
		float y = yPos + aEff.y;

		if (y < -350 || y > 1070 || x < 0 || x > 1280)
			continue;

		if (realI % 2 == 1)
		{

			float skewTL = 0, skewTR = 0;

			float blX = x;
			float brX = x + 4;

			skewTL = (x - lastBottomX);
			skewTR = ((x + 4) - (lastBottomX + 4));

			if (lastBL.a != -1)
			{
				float botxDiff = (x - lastBottomX);
				float botxRDiff = ((x + 4) - (lastBottomX + 4));

				float botdiff = (y - lastBottomY) / 2;

				lastBL.x = lastBottomX + botxDiff;
				lastBR.x = lastBottomX + 4 + botxRDiff;

				points.push_back(lastBL);
				points.push_back(lastTR);
				points.push_back(lastTR);
				points.push_back(lastBL);
				points.push_back(lastBR);

				Rendering::drawBatch();
				Rendering::PushQuad(points, NULL, GL::genShader);
				points.clear();
			}

			float diff = (y - lastBottomY) / 2;

			float xDiff = (x - lastBottomX);
			float xRDiff = ((x + 4) - (lastBottomX + 4));

			if (lastBL.a == -1)
			{
				diff = 0;
				xDiff = 0;
				xRDiff = 0;
			}

			points.push_back({ nextX + xDiff,nextY - diff ,0,0, 255, 255, 255, SplineAlpha}); // tl
			lastTR = { nextX + 4 + xRDiff,nextY - diff,1,0, 255, 255, 255, SplineAlpha }; // tr
			lastBL = { blX,y,0,1, 255, 255, 255, SplineAlpha }; // bl
			lastBR = { brX,y,1,1, 255, 255, 255, SplineAlpha }; // br
			lastBottomX = x;
			lastBottomY = y;
		}
		else
		{
			nextY = y;
			nextX = x;
		}
		realI++;
	}


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