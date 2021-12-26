#include "Judge.h"


std::vector<double> Judge::hitWindows;

std::vector<int> Judge::scoreWindows;

void Judge::initJudge()
{
	// in ms btw!
	hitWindows = {22.5, 45.0, 90.0, 135.0, 180.0};
	// score
	scoreWindows = { 300,250, 100, 50, 0 };
}

int Judge::scoreNote(float difff)
{
	float diff = std::abs(difff);

	for (int i = 0; i < 5; i++)
	{
		double window = hitWindows[i];
		double nextWindow = 0;
		if (i != 0)
			nextWindow = hitWindows[i - 1];

		if (diff > nextWindow && diff <= window)
			return scoreWindows[i];
	}
	return -1;
}

judgement Judge::judgeNote(float difference) {

	float diff = std::abs(difference);

	for (int i = 0; i < 5; i++)
	{
		double window = hitWindows[i];
		double nextWindow = 0;
		if (i != 0)
			nextWindow = hitWindows[i - 1];
		
		if (diff > nextWindow && diff <= window)
			return (judgement)i;
	}
	return (judgement)-1;
}