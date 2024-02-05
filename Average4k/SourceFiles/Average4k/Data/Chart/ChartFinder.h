/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once

#ifndef _AVG4K_CHART_FINDER_H
#define _AVG4K_CHART_FINDER_H
#include <map>
#include "Providers/StepFile.h"
#include <BS_thread_pool.hpp>
#include <BS_thread_pool_utils.hpp>

namespace Average4k::Data
{
	struct Pack {
		std::wstring name;
		std::wstring path;
		std::wstring banner;

		std::vector<Chart::Providers::StepFile> charts;
	};

	class ChartFinder
	{
	public:
		static std::mutex m_lock;
		static std::vector<Pack> Packs;
		static BS::thread_pool pack_pool;

		/// <summary>
		/// Asyncronously find all charts in the given path
		/// </summary>
		/// <param name="path">The path to search</param>
		static void FindCharts(const std::string path);
	};
}

#endif