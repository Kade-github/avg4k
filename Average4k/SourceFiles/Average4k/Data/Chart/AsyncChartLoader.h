/*
	Avg Dev Team
	2021-2024 Average4k
*/

#pragma once
#include <AvgEngine/External/Bass/BASS.h>
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
		size_t size = 0;
		std::wstring path = L"";
	};

	class AsyncChartLoader
	{
	public:
		static std::mutex m_lock;
		static std::mutex s_lock;
		static int currentId;
		static BS::thread_pool pool;
		static BS::thread_pool chartPool;
		static BS::thread_pool audioPool;
		static std::string latestAudioPath;
		static AvgEngine::Audio::Channel* channel;
		static std::unordered_map<int, asyncImage> textures;
		static ChartFile chart;

		static int AsyncLoadTexture(std::wstring path);
		
		static void AsyncLoadChart(std::wstring path);

		static void AsyncLoadAudio(std::string path, std::string name);

		static void LoadAudio(std::string path, std::string name);


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
			if (chartPool.get_tasks_queued() + chartPool.get_tasks_running() > 0)
				return ChartFile();
			ChartFile copy = chart;
			chart = ChartFile();
			return copy;
		}

		static AvgEngine::Audio::Channel* CheckAudio()
		{
			if (audioPool.get_tasks_queued() + audioPool.get_tasks_running() > 0)
				return nullptr;
			std::lock_guard<std::mutex> lock(s_lock);
			if (channel == nullptr)
				return nullptr;
			if (channel->path != latestAudioPath)
				return nullptr;
			return channel;
		}
		
	};
}

#endif