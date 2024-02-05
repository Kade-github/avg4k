/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once
#include <unordered_map>
#include <string>
#include <mutex>
#include <BS_thread_pool.hpp>
#include "ChartFile.h"

#ifndef _AVG4K_ASYNC_CHART_LOADER_H
#define _AVG4K_ASYNC_CHART_LOADER_H

namespace Average4k::Data
{
	struct asyncImage {
		unsigned char* data = nullptr;
		int width = -1;
		int height = -1;
		std::wstring path = L"";
	};

	class AsyncChartLoader
	{
	public:
		static std::mutex m_lock;
		static int currentId;
		static BS::thread_pool pool;
		static std::unordered_map<int, asyncImage> textures;
		static ChartFile chart;

		static int AsyncLoadTexture(std::wstring path);
		
		static void AsyncLoadChart(std::wstring path);

		static void ClearAll();

		static asyncImage checkTexture(int id)
		{
			std::lock_guard<std::mutex> lock(m_lock);
			if (textures.find(id) == textures.end())
				return asyncImage();

			asyncImage copy = textures[id];
			textures[id] = asyncImage();
			return copy;
		}
		static ChartFile CheckChart()
		{
			if (pool.get_tasks_queued() + pool.get_tasks_running() > 0)
				return ChartFile();
			ChartFile copy = chart;
			chart = ChartFile();
			return copy;
		}
	};
}

#endif