#include "Judge.h"


std::vector<double> Judge::hitWindows;

void Judge::initJudge()
{
	// in ms btw!
	hitWindows = {22.5, 45.0, 90.0, 135.0, 180.0};
}

judgement Judge::judgeNote(float difference) {

	float diff = std::abs(difference);

	for (int i = 0; i < 5; i++)
	{
		double window = hitWindows[i];
		double nextWindow = 0;
		if (i == 0)
			nextWindow = 0;
		else
			nextWindow = hitWindows[i - 1];
		

		if (diff > nextWindow && diff <= window)
			return (judgement)i;
	}
	return (judgement)-1;
}