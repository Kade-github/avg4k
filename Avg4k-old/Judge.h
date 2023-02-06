#pragma once
#include "includes.h"
enum class judgement {
	Judge_marvelous = 0,
	Judge_perfect = 1,
	Judge_great = 2,
	Judge_good = 3,
	Judge_bad = 4,
	Judge_miss = -1
};

class Judge
{

	public:
		static std::vector<double> hitWindows;
		static std::vector<int> scoreWindows;
		static void initJudge();

		static judgement judgeNote(float difff);
		static int scoreNote(float diff);
};

