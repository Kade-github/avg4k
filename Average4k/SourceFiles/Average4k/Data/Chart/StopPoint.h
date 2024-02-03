/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_STOP_POINT_H
#define _AVG4K_STOP_POINT_H

#include <iostream>

namespace Average4k::Data::Chart
{
	class StopPoint
	{
	public:
		float startBeat = -1;
		float startTimestamp = -1;
		float length = -1;
	};
}

#endif