/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_CHART_FINDER_H
#define _AVG4K_CHART_FINDER_H

#include "Providers/StepFile.h"

namespace Average4k::Data
{
	class ChartFinder
	{
	public:
		static std::mutex m_lock;
		static std::vector<ChartFile> Charts;

		/// <summary>
		/// Asyncronously find all charts in the given path
		/// </summary>
		/// <param name="path">The path to search</param>
		static void FindCharts(const std::string path);
	};
}

#endif