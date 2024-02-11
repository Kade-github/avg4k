/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_SCROLL_POINT_H
#define _AVG4K_SCROLL_POINT_H

#include <iostream>

namespace Average4k::Data::Chart
{
	class ScrollPoint
	{
	public:
		float startBeat = -1;
		float scrollMultiplier = 1;
	};
}

#endif