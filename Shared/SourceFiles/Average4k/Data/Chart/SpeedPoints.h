/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_SPEED_POINT_H
#define _AVG4K_SPEED_POINT_H

#include <iostream>

namespace Average4k::Data::Chart
{
	class SpeedPoint
	{
	public:
		float startBeat = -1;
		float endBeat = -1;
		float startTimestamp = -1;
		float endTimestamp = -1;
		float stretch = 1;
		float delay = 0;
		int type = 0;

		int spdId = 0;
	};
}

#endif