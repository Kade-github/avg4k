/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_TIMING_POINT_H
#define _AVG4K_TIMING_POINT_H

#include <iostream>

namespace Average4k::Data::Chart
{
	class TimingPoint
	{
	public:
		float bpm = 120;
		float startBeat = 0;
		float endBeat = INFINITY;

		float startTimestamp = 0;
		float endTimestamp = INFINITY;
		float length = INFINITY;
	};
}

#endif